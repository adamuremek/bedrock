/**
 * ======================================DESCRIPTION======================================
 *
 * This file contains many helper methods for interprocess communication.
 * Specifically, it provides functionality for easily spinning up virtual networks,
 * spawning processes, and sending test results (in bulk) between processes as part of
 * completing multi-process tests.
 *
 * =======================================================================================
 */

#ifndef TESTS_IPC_H
#define TESTS_IPC_H

#include <functional>
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <utility>
#include <vector>
#include <map>

using namespace std;

class VirtualNetwork{
private:
    inline static vector<string> bridges;
    inline static vector<string> namespaces;
    inline static map<string, vector<pair<string, string>>> vethDevices;

    static bool destroyNetNamespace(const string& nsName){
        // Delete namespace
        string cmd = "ip netns del " + nsName;
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to delete namespace: " << nsName << endl;
            return false;
        }

        return true;
    }

    static bool destroyVethDevice(const string& nsName, const pair<string, string>& vethDevice){
        // Take down the veth
        string cmd = "ip netns exec " + nsName + " ip link set " + vethDevice.first + " down";
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to take down veth: " << nsName << endl;
            return false;
        }

        // Remove the veth device
        cmd = "ip link del " + vethDevice.second;
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to take down veth: " << vethDevice.second << endl;
            return false;
        }

        return true;
    }

    static bool destroyVirtualBridge(const string& brName){
        // Bring down the bridge device
        string cmd = "ip link set dev " + brName + " down";
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to bring up bridge: " << brName << endl;
            return false;
        }

        // Destroy the virtual bridge
        cmd = "ip link del " + brName;
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to destroy bridge: " << brName << endl;
            return false;
        }

        return true;
    }

public:

    static bool createNetNamespace(const string& nsName){
        // Create the namespace
        string cmd = "ip netns add " + nsName;
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to create namespace: " << nsName << endl;
            return false;
        }

        namespaces.push_back(nsName);
        return true;
    }

    static bool createVirtualBridge(const string& brName){
        // Create the virtual bridge
        string cmd = "ip link add " + brName + " type bridge";
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to create bridge: " << brName << endl;
            return false;
        }

        // Bring up the bridge device
        cmd = "ip link set dev " + brName + " up";
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to bring up bridge: " << brName << endl;
            return false;
        }

        bridges.push_back(brName);
        return true;
    }

    static bool connectNSToBridge(const string& nsName, const string& brName, const string& bindAddr){
        // Create a link to connect the ns to the bridge
        string vethHeadName = "veth-" + nsName;
        string vethTailName = "veth-br-" + nsName;
        string cmd = "ip link add " + vethHeadName + " type veth peer name " + vethTailName;
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to bridge link for " << brName << endl;
            return false;
        }

        // Attach head of the veth to the namespace
        cmd = "ip link set " + vethHeadName + " netns " + nsName;
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to attach veth to namespace: " << nsName << endl;
            return false;
        }

        // Attach the tail of the veth to the bridge
        cmd = "ip link set " + vethTailName + " master " + brName;
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to attach veth to bridge: " << brName << endl;
            return false;
        }

        // Bring the tail veth up
        cmd = "ip link set " + vethTailName + " up";
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to bring up tail veth: " << nsName << endl;
            return false;
        }

        // Configure the IP address of the veth head
        cmd = "ip netns exec " + nsName + " ip  addr add " + bindAddr + " dev " + vethHeadName;
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to bind ip address to head of veth: " << nsName + " " + bindAddr << endl;
            return false;
        }

        // Bring the veth head up
        cmd = "ip netns exec " + nsName + " ip link set " + vethHeadName + " up";
        if(system(cmd.c_str()) != 0){
            cerr << "Failed to bring up veth: " << nsName << endl;
            return false;
        }

        vethDevices[nsName].emplace_back(vethHeadName, vethTailName);
        return true;
    }

    static void resetNetwork(){
        for(const auto& kv : vethDevices){
            for(const auto& pair : kv.second){
                // Destroy veth device
                destroyVethDevice(kv.first, pair);
            }
        }

        for(const auto& nsName : namespaces){
            // Destroy network namespace
            destroyNetNamespace(nsName);
        }

        for(const auto& bridge : bridges){
            destroyVirtualBridge(bridge);
        }

        // Clear all stored data
        namespaces.clear();
        bridges.clear();
        vethDevices.clear();
    }
    
};

namespace IPC{
    static vector<pid_t> spawnedProcs;

    namespace {
        class BehaviorRegistry{
        private:
            std::map<std::string, std::function<void()>> functions;
            BehaviorRegistry(){}
        public:


            static BehaviorRegistry& getInstance(){
                static BehaviorRegistry instance;
                return instance;
            }

            void registerBehavior(const std::string& name, const std::function<void()>& func){
                functions.insert(std::make_pair(name, func));
            }

            void useBehavior(const std::string& name){
                auto it = functions.find(name);
                if(it != functions.end()){
                    it->second();
                }else{
                    std::cerr << "Behavior not found: " << name << std::endl;
                }
            }
        };
    }

    inline void registerBehavior(const std::string& name, const std::function<void()>& func){
        BehaviorRegistry::getInstance().registerBehavior(name, func);
    }

    inline void useBehavior(const std::string& name){
        BehaviorRegistry::getInstance().useBehavior(name);
    }

    void spawnProcess(const string& behavior){
        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "Fork failed." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            // Execute the specified behavior
            useBehavior(behavior);

            // Exit out fo the child process
            exit(EXIT_SUCCESS);
        }

        spawnedProcs.push_back(pid);
    }

    void spawnProcess(const string& namespacePath, const string& behavior) {
        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "Fork failed." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            // Open the network namespace file in the child to avoid affecting the parent
            int fd = open(namespacePath.c_str(), O_RDONLY);
            if (fd == -1) {
                std::cerr << "Failed to open namespace file" << std::endl;
                exit(EXIT_FAILURE);
            }

            // Set the network namespace in the child
            if (setns(fd, CLONE_NEWNET) == -1) {
                std::cerr << "Failed to set namespace" << std::endl;
                close(fd);
                exit(EXIT_FAILURE);
            }
            close(fd);

            // Execute the specified behavior
            useBehavior(behavior);

            // Exit out fo the child process
            exit(EXIT_SUCCESS);
        }

        spawnedProcs.push_back(pid);
    }

    void waitForAllProcesses(){
        for(const auto& pid : spawnedProcs){
            int status;
            waitpid(pid, &status, 0);
        }

        spawnedProcs.clear();
    }
}

#endif //TESTS_IPC_H
