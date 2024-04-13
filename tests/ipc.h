/**
 * =============DESCRIPTION=============
 *
 * This file contains many helper methods for interprocess communication.
 * Specifically, it provides functionality for easily spinning up virtual networks,
 * spawning processes, and sending test results (in bulk) between processes as part of
 * completing multi-process tests.
 */

#ifndef TESTS_IPC_H
#define TESTS_IPC_H

#include "behavior_registry.h"
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <vector>

class VirtualNetwork{
private:
    inline static std::vector<std::string> bridges;
public:

    static bool createVirtualBridge(const std::string& brName){
        // Create the virtual bridge
        std::string cmd = "ip link add " + brName + " type bridge";
        if(system(cmd.c_str()) != 0){
            std::cerr << "Failed to create bridge: " << brName << std::endl;
            return false;
        }

        // Bring up the bridge device
        cmd = "ip link set dev " + brName + " up";
        if(system(cmd.c_str()) != 0){
            std::cerr << "Failed to bring up bridge: " << brName << std::endl;
            return false;
        }

        bridges.push_back(brName);
        return true;
    }
};



bool setNetworkNamespace(const char* nsPath) {
    int nsFd = open(nsPath, O_RDONLY);  // Open the namespace file
    if (nsFd == -1) {
        perror("Failed to open namespace");
        return false;
    }

    if (setns(nsFd, CLONE_NEWNET) == -1) {  // Set the network namespace
        perror("Failed to set namespace");
        close(nsFd);
        return false;
    }

    close(nsFd);
    return true;
}

void spawnApp(){

}


#endif //TESTS_IPC_H
