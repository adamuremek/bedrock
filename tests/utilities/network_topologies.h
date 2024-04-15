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



#endif //BEDROCK_NETWORK_TOPOLOGIES_H
