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
#include <sys/mman.h>
#include <gtest/gtest.h>
#include <semaphore.h>
#include <thread>
#include <chrono>

using namespace std;

class VirtualNetwork {
private:
    inline static vector<string> bridges;
    inline static vector<string> namespaces;
    inline static map<string, vector<pair<string, string>>> vethDevices;

    static bool destroyNetNamespace(const string &nsName) {
        // Delete namespace
        string cmd = "ip netns del " + nsName;
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to delete namespace: " << nsName << endl;
            return false;
        }

        return true;
    }

    static bool destroyVethDevice(const string &nsName, const pair<string, string> &vethDevice) {
        // Take down the veth
        string cmd = "ip netns exec " + nsName + " ip link set " + vethDevice.first + " down";
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to take down veth: " << nsName << endl;
            return false;
        }

        // Remove the veth device
        cmd = "ip link del " + vethDevice.second;
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to take down veth: " << vethDevice.second << endl;
            return false;
        }

        return true;
    }

    static bool destroyVirtualBridge(const string &brName) {
        // Bring down the bridge device
        string cmd = "ip link set dev " + brName + " down";
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to bring up bridge: " << brName << endl;
            return false;
        }

        // Destroy the virtual bridge
        cmd = "ip link del " + brName;
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to destroy bridge: " << brName << endl;
            return false;
        }

        return true;
    }

public:

    static bool createNetNamespace(const string &nsName) {
        // Create the namespace
        string cmd = "ip netns add " + nsName;
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to create namespace: " << nsName << endl;
            return false;
        }

        namespaces.push_back(nsName);
        return true;
    }

    static bool createVirtualBridge(const string &brName) {
        // Create the virtual bridge
        string cmd = "ip link add " + brName + " type bridge";
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to create bridge: " << brName << endl;
            return false;
        }

        // Bring up the bridge device
        cmd = "ip link set dev " + brName + " up";
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to bring up bridge: " << brName << endl;
            return false;
        }

        bridges.push_back(brName);
        return true;
    }

    static bool connectNSToBridge(const string &nsName, const string &brName, const string &bindAddr) {
        // Create a link to connect the ns to the bridge
        string vethHeadName = "veth-" + nsName;
        string vethTailName = "veth-br-" + nsName;
        string cmd = "ip link add " + vethHeadName + " type veth peer name " + vethTailName;
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to bridge link for " << brName << endl;
            return false;
        }

        // Attach head of the veth to the namespace
        cmd = "ip link set " + vethHeadName + " netns " + nsName;
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to attach veth to namespace: " << nsName << endl;
            return false;
        }

        // Attach the tail of the veth to the bridge
        cmd = "ip link set " + vethTailName + " master " + brName;
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to attach veth to bridge: " << brName << endl;
            return false;
        }

        // Bring the tail veth up
        cmd = "ip link set " + vethTailName + " up";
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to bring up tail veth: " << nsName << endl;
            return false;
        }

        // Configure the IP address of the veth head
        cmd = "ip netns exec " + nsName + " ip  addr add " + bindAddr + " dev " + vethHeadName;
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to bind ip address to head of veth: " << nsName + " " + bindAddr << endl;
            return false;
        }

        // Bring the veth head up
        cmd = "ip netns exec " + nsName + " ip link set " + vethHeadName + " up";
        if (system(cmd.c_str()) != 0) {
            cerr << "Failed to bring up veth: " << nsName << endl;
            return false;
        }

        vethDevices[nsName].emplace_back(vethHeadName, vethTailName);
        return true;
    }

    static void resetNetwork() {
        for (const auto &kv: vethDevices) {
            for (const auto &pair: kv.second) {
                // Destroy veth device
                destroyVethDevice(kv.first, pair);
            }
        }

        for (const auto &nsName: namespaces) {
            // Destroy network namespace
            destroyNetNamespace(nsName);
        }

        for (const auto &bridge: bridges) {
            destroyVirtualBridge(bridge);
        }

        // Clear all stored data
        namespaces.clear();
        bridges.clear();
        vethDevices.clear();
    }

};

namespace IPC {
    namespace {
        vector<pid_t> spawnedProcs;

        //=======================NON-VISIBLE DATA FOR TEST RESULTS=======================//
        struct Result {
            char testDescription[100];
            bool result;
        };

        Result *results = nullptr;
        int *currentTest = nullptr;

        int shmResultsFd = 0;
        int shmCurrentTestFd = 0;

        size_t shmResultsSize = 0;
        size_t shmCurrentTestSize = sizeof(int);

        const char *shmResultsName = "/test_results_shm";
        const char *shmCurrentTestName = "/test_current_test_shm";

        sem_t *sem = nullptr;
        const char *semName = "/test_results_sem";
        int maxTestNum = 0;
        //===============================================================================//


        class BehaviorRegistry {
        private:
            std::map<std::string, std::function<void()>> functions;

            BehaviorRegistry() {}

        public:


            static BehaviorRegistry &getInstance() {
                static BehaviorRegistry instance;
                return instance;
            }

            void registerBehavior(const std::string &name, const std::function<void()> &func) {
                functions.insert(std::make_pair(name, func));
            }

            void useBehavior(const std::string &name) {
                auto it = functions.find(name);
                if (it != functions.end()) {
                    it->second();
                } else {
                    std::cerr << "Behavior not found: " << name << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        };
    }

    static void startTests(int numTests) {
        // Open shared memory file descriptor
        shmResultsFd = shm_open(shmResultsName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        shmCurrentTestFd = shm_open(shmCurrentTestName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (shmResultsFd == -1 || shmCurrentTestFd == -1) {
            cerr << "Failed to open shared memory." << endl;
            return;
        }

        // Size up the descriptor and map the memory to the results object
        shmResultsSize = sizeof(Result) * numTests;
        if (ftruncate(shmResultsFd, shmResultsSize) == -1 || ftruncate(shmCurrentTestFd, shmCurrentTestSize) == -1) {
            cerr << "Failed to ftruncate shared memory." << endl;
            close(shmResultsFd);
            close(shmCurrentTestFd);
            return;
        }

        results = (Result *) mmap(nullptr, shmResultsSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmResultsFd, 0);
        currentTest = (int *) mmap(nullptr, shmCurrentTestSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmCurrentTestFd,0);

        if(results == MAP_FAILED){
            cout << "FUCK" << endl;
        }

        if(currentTest == MAP_FAILED){
            cout << "SHIT" << endl;
        }

        if (results == MAP_FAILED || currentTest == MAP_FAILED) {
            cerr << "Failed to map shared memory." << endl;
            close(shmResultsFd);
            close(shmCurrentTestFd);
            return;
        }

        //Initialize the memory area
        memset(results, 0, shmResultsSize);
        memset(currentTest, 0, shmCurrentTestSize);

        maxTestNum = numTests;

        // Create a semaphore for locking the section of code that is used for adding test data
        sem = sem_open(semName, O_CREAT, 0644, 1);
        if (sem == SEM_FAILED) {
            cerr << "Failed to open semaphore." << endl;
            return;
        }
    }

    static void endTests() {
        if(!results && !currentTest){
            return;
        }

        // Cleanup shared memory data
        munmap(results, shmResultsSize);
        close(shmResultsFd);
        shm_unlink(shmResultsName);

        munmap(currentTest, shmCurrentTestSize);
        close(shmCurrentTestFd);
        shm_unlink(shmCurrentTestName);

        // Cleanup semaphore
        sem_close(sem);
        sem_unlink(semName);

        // Reset test/memory data
        results = nullptr;
        currentTest = nullptr;

        shmResultsFd = 0;
        shmCurrentTestFd = 0;
        shmResultsSize = 0;

        maxTestNum = 0;
    }

    static void addTestResult(const char *msg, bool result) {
        if (sem_wait(sem) != 0) {
            cerr << "Sem wait failed." << endl;
            return;
        }

        if (strlen(msg) > 100) {
            cerr << "Result message cannot be longer than 100 characters." << endl;
            return;
        }

        if (*currentTest >= maxTestNum) {
            cerr << "Test results have reached max capacity! Cannot add any more test results." << endl;
            return;
        }

        // Store test information
        strcpy(results[*currentTest].testDescription, msg);
        results[*currentTest].result = result;

        *currentTest += 1;

        if (sem_post(sem) != 0) {
            cerr << "Sem post failed." << endl;
            return;
        }
    }

    static void evaluateTestResults() {
        for (int i = 0; i < *currentTest; i++) {
            ASSERT_TRUE(results[i].result) << "Test: " << results[i].testDescription;
        }
    }

    inline void registerBehavior(const std::string &name, const std::function<void()> &func) {
        BehaviorRegistry::getInstance().registerBehavior(name, func);
    }

    inline void useBehavior(const std::string &name) {
        BehaviorRegistry::getInstance().useBehavior(name);
    }

    void spawnProcess(const string &behavior) {
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

    void spawnProcess(const string &nsName, const string &behavior) {
        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "Fork failed." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            // Open the network namespace file in the child to avoid affecting the parent
            string nsPath = "/var/run/netns/" + nsName;
            int fd = open(nsPath.c_str(), O_RDONLY);
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

    void waitForAllProcesses() {
        for (const auto &pid: spawnedProcs) {
            int status;
            waitpid(pid, &status, 0);

            int exit_status = WEXITSTATUS(status);
            if(exit_status == EXIT_FAILURE){
                FAIL() << "Process terminated unsuccessfully.";
            }
        }

        spawnedProcs.clear();
    }

    void sleep(int milliseconds){
        this_thread::sleep_for(chrono::milliseconds(milliseconds));
    }

    void waitForSignal(const string& signalName){
        sem_t *sig = sem_open(signalName.c_str(), O_CREAT, 0644, 0);

        if(sig == SEM_FAILED){
            cerr << "sem_open failed in 'waitForSignal'" << endl;
            return;
        }

        if(sem_wait(sig) == -1){
            cerr << "sem_wait failed in 'waitForSignal'" << endl;
            return;
        }

        sem_close(sig);
        sem_unlink(semName);
    }

    void postSignal(const string& signalName){
        sem_t *sig = sem_open(signalName.c_str(), O_CREAT, 0644, 0);

        if(sig == SEM_FAILED){
            cerr << "sem_open failed in 'postSignal'" << endl;
            return;
        }

        if(sem_post(sig) == -1){
            cerr << "sem_post failed in 'postSignal'" << endl;
            return;
        }

        sem_close(sig);
    }

}

#endif //TESTS_IPC_H
