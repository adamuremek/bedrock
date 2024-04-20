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
                        if(Bedrock::actingAs == ACTOR_CLIENT){
                            onHostConnect.invoke();
                        } else if(Bedrock::actingAs == ACTOR_SERVER){
                            onClientConnect.invoke();
                        }
                        break;
                    }
                    case ENET_EVENT_TYPE_DISCONNECT: {
                        if(Bedrock::actingAs == ACTOR_CLIENT){
                            onHostDisconnect.invoke();
                            eventLoopActive = false;
                        }
                        else if(Bedrock::actingAs == ACTOR_SERVER){
                            onClientDisconnect.invoke();
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

Bedrock::Event<> Bedrock::onClientConnect;
Bedrock::Event<> Bedrock::onClientDisconnect;
Bedrock::Event<> Bedrock::onHostConnect;
Bedrock::Event<> Bedrock::onHostDisconnect;
bool Bedrock::isInitialized = false;


bool Bedrock::init() {
    isInitialized = enet_initialize() == 0;
    return isInitialized;
}

void Bedrock::shutdown() {
    // Shutdown if currently acting as the server
    if(Bedrock::actingAs == ACTOR_SERVER){
        eventLoopActive = false;
        eventLoop.join();

        enet_host_destroy(enetHost);
    }else if(Bedrock::actingAs == ACTOR_CLIENT){
        enet_peer_disconnect(enetPeer, 0);
        enet_host_flush(enetHost);

        eventLoop.join();

        enet_host_destroy(enetHost);
    }

    enet_deinitialize();
    clearEventCallbacks();
    Bedrock::actingAs = ACTOR_NONE;
    isInitialized = false;
}


bool Bedrock::startDedicatedHost(uint16_t port) {
    ENetAddress addr{};
    addr.port = port;

    enetHost = enet_host_create(&addr, 32, 2, 0, 0);

    if(enetHost == nullptr){
        std::cout << "ASS"<< std::endl;
    }

    Bedrock::actingAs = ACTOR_SERVER;

    eventLoopActive = true;
    eventLoop = std::thread(Bedrock::pollEvents);

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

void Bedrock::clearEventCallbacks() {
    onClientConnect.clear();
    onClientDisconnect.clear();
    onHostConnect.clear();
    onHostDisconnect.clear();
}
