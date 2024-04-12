#include "bedrock/bedrock.h"
#include <iostream>


namespace Bedrock{
    // This here is just to make the pollEvents function exclusive to Bedrock internally.
    // We really don't want to give users the ability to manually call this function.
    namespace{
        void pollEvents() {
            ENetEvent event;

            while (eventLoopActive) {
                enet_host_service(enetHost, &event, 0);

                switch (event.type) {
                    case ENET_EVENT_TYPE_NONE:{
                        break;
                    }
                    case ENET_EVENT_TYPE_CONNECT: {
                        clientConnectedToHost.invoke();
                        break;
                    }
                    case ENET_EVENT_TYPE_DISCONNECT: {
                        clientDisconnectedFromHost.invoke();

                        if(Bedrock::actingAs == ACTOR_CLIENT){
                            eventLoopActive = false;
                        }

                        break;
                    }
                    case ENET_EVENT_TYPE_RECEIVE: {
                        //debug start
                        if(Bedrock::actingAs == ACTOR_CLIENT){
                            std::cout << "Message received from host" << std::endl;
                        }else if(Bedrock::actingAs == ACTOR_SERVER){
                            std::cout << "Message received from client" << std::endl;

                        }
                        //debug end

                        Message incomingMsg{};
                        Message outgoingMsg{};

                        outgoingMsg.data = event.packet->data;
                        outgoingMsg.size = event.packet->dataLength;

                        executeMsgCallback(incomingMsg, outgoingMsg);

                        enet_packet_destroy(event.packet);
                        break;
                    }
                }
            }

            std::cout << "===CLIENT POLL END===" << std::endl;
        }
    }
}

bool Bedrock::init() {
    enet_initialize();

    return true;
}

bool Bedrock::shutdown() {
    enet_deinitialize();
    clearEventCallbacks();
    return true;
}

bool Bedrock::startDedicatedHost(uint16_t port) {
    ENetAddress addr{};
    addr.port = port;

    enetHost = enet_host_create(&addr, 32, 2, 0, 0);

    if(enetHost == nullptr){
        std::cout << "ASS"<< std::endl;
    }


    eventLoopActive = true;
    eventLoop = std::thread(Bedrock::pollEvents);

    return true;
}

bool Bedrock::stopDedicatedHost() {
    eventLoopActive = false;
    eventLoop.join();
    enet_host_destroy(enetHost);

    return true;
}

bool Bedrock::startClient(uint16_t port, const char* host) {
    enetHost = enet_host_create(nullptr, 1, 2, 0, 0);

    ENetAddress addr{};
    enet_address_set_host(&addr, host);
    addr.port = port;

    enetPeer = enet_host_connect(enetHost, &addr, 2, 0);

    eventLoopActive = true;
    eventLoop = std::thread(Bedrock::pollEvents);

    return true;
}

bool Bedrock::stopClient() {
    enet_peer_disconnect(enetPeer, 0);
    enet_host_flush(enetHost);

    eventLoop.join();

    enet_host_destroy(enetHost);
    return true;
}

void Bedrock::clearEventCallbacks() {
    clientConnectedToHost.clear();
    clientDisconnectedFromHost.clear();
}
