#include "bedrock/bedrock.h"
#include <iostream>


namespace Bedrock{
    // This here is just to make the pollEvents function exclusive to Bedrock internally.
    // We really don't want to give users the ability to manually call this function.
    namespace{
        void pollEvents() {
            ENetEvent event;
            BedrockMetadata& metadata = BedrockMetadata::getInstance();

            while (metadata.getEventLoopActive()) {
                enet_host_service(metadata.getEnetHost(), &event, 10);

                switch (event.type) {
                    case ENET_EVENT_TYPE_NONE:{
                        break;
                    }
                    case ENET_EVENT_TYPE_CONNECT: {
                        if(metadata.isRole(ACTOR_CLIENT)){
                            onHostConnect.invoke();
                        } else if(metadata.isRole(ACTOR_SERVER)){
                            auto clientId = static_cast<ClientID>(event.peer - metadata.getEnetHost()->peers);

                            onClientConnect.invoke(clientId);
                        }
                        break;
                    }
                    case ENET_EVENT_TYPE_DISCONNECT: {
                        if(metadata.isRole(ACTOR_CLIENT)){
                            onHostDisconnect.invoke();
                            metadata.setEventLoopActive(false);
                        }
                        else if(metadata.isRole(ACTOR_SERVER)){
                            auto clientId = static_cast<ClientID>(event.peer - metadata.getEnetHost()->peers);

                            onClientDisconnect.invoke(clientId);
                            removeClientFromLayers(clientId);
                        }
                        break;
                    }
                    case ENET_EVENT_TYPE_RECEIVE: {
                        Message incomingMsg{};
                        Message outgoingMsg{};


                        incomingMsg.data = event.packet->data;
                        incomingMsg.size = event.packet->dataLength;

                        executeMsgCallback(incomingMsg, outgoingMsg);

                        if(metadata.isRole(ACTOR_CLIENT)){
                            if(outgoingMsg.data != nullptr){
                                sendMessageToHost(outgoingMsg);
                            }
                        }else if(metadata.isRole(ACTOR_SERVER)){
                            if(outgoingMsg.data != nullptr){
                                auto clientId = static_cast<ClientID>(event.peer - metadata.getEnetHost()->peers);
                                sendMessageToClient(outgoingMsg, clientId);
                            }
                        }

                        enet_packet_destroy(event.packet);

                        break;
                    }
                }
            }
        }
    }
}


BEDROCK_API Bedrock::Event<void, const Bedrock::ClientID&> Bedrock::onClientConnect;
BEDROCK_API Bedrock::Event<void, const Bedrock::ClientID&> Bedrock::onClientDisconnect;
BEDROCK_API Bedrock::Event<void> Bedrock::onHostConnect;
BEDROCK_API Bedrock::Event<void> Bedrock::onHostDisconnect;
BEDROCK_API bool Bedrock::isInitialized = false;


Bedrock::StatusCode Bedrock::init() {
    isInitialized = enet_initialize() == 0;
    return isInitialized ? StatusCode::SUCCESS : StatusCode::INITIALIZATION_FAILED;
}

Bedrock::StatusCode Bedrock::shutdown() {
    if(!isInitialized){
        return StatusCode::NOT_INITIALIZED;
    }

    BedrockMetadata& metadata = BedrockMetadata::getInstance();
    
    // Shutdown if currently acting as the server
    if(metadata.isRole(ACTOR_SERVER)){
        metadata.setEventLoopActive(false);
        metadata.eventLoop.join();

        enet_host_destroy(metadata.getEnetHost());
    }else if(metadata.isRole(ACTOR_CLIENT)){
        enet_peer_disconnect(metadata.getEnetPeer(), 0);
        enet_host_flush(metadata.getEnetHost());

        metadata.eventLoop.join();
        enet_host_destroy(metadata.getEnetHost());
    }

    enet_deinitialize();
    clearEventCallbacks();
    metadata.setRole(ACTOR_NONE);
    isInitialized = false;

    return StatusCode::SUCCESS;
}

bool Bedrock::isRole(const Bedrock::Role &roleQuery) {
    return Bedrock::BedrockMetadata::getInstance().isRole(roleQuery);
}

Bedrock::StatusCode Bedrock::startDedicatedHost(uint16_t port) {
    if(!isInitialized){
        return StatusCode::NOT_INITIALIZED;
    }

    BedrockMetadata& metadata = BedrockMetadata::getInstance();

    ENetAddress addr{};
    addr.port = port;

    ENetHost* host = enet_host_create(&addr, 32, 2, 0, 0);
    if(host == nullptr){
        return StatusCode::CREATE_HOST_FAILED;
    }else{
        metadata.setEnetHost(host);
    }

    metadata.setRole(ACTOR_SERVER);
    metadata.setEventLoopActive(true);
    metadata.eventLoop = std::thread(Bedrock::pollEvents);

    return StatusCode::SUCCESS;
}

Bedrock::StatusCode Bedrock::startClient(uint16_t port, const char* hostAddr) {
    if(!isInitialized){
        return StatusCode::NOT_INITIALIZED;
    }

    BedrockMetadata& metadata = BedrockMetadata::getInstance();

    ENetHost* host = enet_host_create(nullptr, 1, 2, 0, 0);
    metadata.setEnetHost(host);

    ENetAddress addr{};
    enet_address_set_host(&addr, hostAddr);
    addr.port = port;

    metadata.setRole(ACTOR_CLIENT);

    ENetPeer* peer = enet_host_connect(host, &addr, 2, 0);
    if(peer == nullptr){
        return StatusCode::CREATE_HOST_FAILED;
    }else{
        metadata.setEnetPeer(peer);
    }

    metadata.setEventLoopActive(true);
    metadata.eventLoop = std::thread(Bedrock::pollEvents);

    return StatusCode::SUCCESS;
}

void Bedrock::clearEventCallbacks() {
    onClientConnect.clear();
    onClientDisconnect.clear();
    onHostConnect.clear();
    onHostDisconnect.clear();
}


Bedrock::StatusCode Bedrock::sendMessageToHost(const Bedrock::Message &msg) {
    ENetPacket* packet = enet_packet_create(msg.data,
                                            msg.size,
                                            ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);

    packet->freeCallback = deleteMessageData;
    enet_peer_send(BedrockMetadata::getInstance().getEnetPeer(), 0, packet);
    enet_host_flush(BedrockMetadata::getInstance().getEnetHost());

    return StatusCode::SUCCESS;
}


Bedrock::StatusCode Bedrock::sendMessageToClient(const Message& msg, ClientID client){
    ENetPacket* packet = enet_packet_create(msg.data,
                                            msg.size,
                                            ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);

    packet->freeCallback = deleteMessageData;

    ENetHost* host = BedrockMetadata::getInstance().getEnetHost();
    ENetPeer* peer = &host->peers[client];
    enet_peer_send(peer, 0, packet);
    enet_host_flush(host);

    return StatusCode::SUCCESS;
}

Bedrock::StatusCode Bedrock::sendMessageToLayer(const Bedrock::Message &msg, Bedrock::LayerId layer) {
    ENetPacket* packet = enet_packet_create(msg.data,
                                            msg.size,
                                            ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);

    packet->freeCallback = deleteMessageData;

    ENetHost* host = BedrockMetadata::getInstance().getEnetHost();

    for(const auto& client : getIdsFromLayer(layer)){
        ENetPeer* peer = &host->peers[client];
        enet_peer_send(peer, 0, packet);
    }

    enet_host_flush(host);

    return StatusCode::SUCCESS;
}