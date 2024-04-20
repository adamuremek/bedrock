#ifndef BEDROCK_NETWORK_TOPOLOGIES_H
#define BEDROCK_NETWORK_TOPOLOGIES_H

#include "ipc.h"

/**
 *
 *
 */
void topology1(){
    VirtualNetwork::createVirtualBridge("br0");
    VirtualNetwork::createNetNamespace("Host1");
    VirtualNetwork::createNetNamespace("Host2");

    VirtualNetwork::connectNSToBridge("Host1", "br0", "10.0.0.1/24");
    VirtualNetwork::connectNSToBridge("Host2", "br0", "10.0.0.2/24");
}


void topology2(){
    VirtualNetwork::createVirtualBridge("br0");
    VirtualNetwork::createNetNamespace("Client");
    VirtualNetwork::createNetNamespace("Server");

    VirtualNetwork::connectNSToBridge("Server", "br0", "10.0.0.1/24");
    VirtualNetwork::connectNSToBridge("Client", "br0", "10.0.0.2/24");
}

void topology3(){
    VirtualNetwork::createVirtualBridge("br0");
    VirtualNetwork::createNetNamespace("Client1");
    VirtualNetwork::createNetNamespace("Client2");
    VirtualNetwork::createNetNamespace("Client3");
    VirtualNetwork::createNetNamespace("Client4");
    VirtualNetwork::createNetNamespace("Client5");
    VirtualNetwork::createNetNamespace("Server");

    VirtualNetwork::connectNSToBridge("Server", "br0", "10.0.0.1/24");
    VirtualNetwork::connectNSToBridge("Client1", "br0", "10.0.0.2/24");
    VirtualNetwork::connectNSToBridge("Client2", "br0", "10.0.0.3/24");
    VirtualNetwork::connectNSToBridge("Client3", "br0", "10.0.0.4/24");
    VirtualNetwork::connectNSToBridge("Client4", "br0", "10.0.0.5/24");
    VirtualNetwork::connectNSToBridge("Client5", "br0", "10.0.0.6/24");
}


#endif //BEDROCK_NETWORK_TOPOLOGIES_H
