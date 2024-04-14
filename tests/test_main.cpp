#include "ipc.h"

#include <iostream>
#include "behaviors.h"

int main(){
//    VirtualNetwork::createVirtualBridge("v-br0");
//    VirtualNetwork::createNetNamespace("ns-1");
//    VirtualNetwork::createNetNamespace("ns-2");
//    VirtualNetwork::connectNSToBridge("ns-1", "v-br0", "10.0.0.1/24");
//    VirtualNetwork::connectNSToBridge("ns-2", "v-br0", "10.0.0.2/24");
//    VirtualNetwork::resetNetwork();


    registerBehaviors();

    IPC::spawnProcess("clientBehavior1");
    IPC::spawnProcess("clientBehavior1");
    IPC::spawnProcess("clientBehavior1");
    IPC::spawnProcess("clientBehavior1");
    IPC::spawnProcess("clientBehavior2");


    IPC::waitForAllProcesses();

}