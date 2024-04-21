#ifndef BEDROCK_BEDROCK_H
#define BEDROCK_BEDROCK_H

#include "types.h"
#include "serialization.h"
#include "event.h"
#include "messaging.h"
#include <enet/enet.h>
#include <thread>
#include <iostream>

#define ACTOR_NONE static_cast<unsigned char>(0)
#define ACTOR_CLIENT static_cast<unsigned char>(1)
#define ACTOR_SERVER static_cast<unsigned char>(2)


namespace Bedrock{
    // Non-visible declarations (outside of Bedrock)
    namespace{

        inline void deleteMessageData(ENetPacket* packet){
            delete packet->data;
        }
    }

    class BedrockMetadata{
    private:
        ENetHost* enetHost = nullptr;
        ENetPeer* enetPeer = nullptr;
        bool eventLoopActive = false;

        Role role = ACTOR_NONE;
    public:
        std::thread eventLoop;

        static BedrockMetadata& getInstance(){
            static BedrockMetadata instance;
            return instance;
        }

        inline bool isRole(const Role& roleQuery) { return roleQuery == role; }

        // Getters
        inline ENetHost* getEnetHost() const {return enetHost; }
        inline ENetPeer* getEnetPeer() const { return enetPeer; }
        inline bool getEventLoopActive() const { return eventLoopActive; }

        // Setters
        inline void setEnetHost(ENetHost* host) { enetHost = host; }
        inline void setEnetPeer(ENetPeer* peer) { enetPeer = peer; }
        inline void setEventLoopActive(const bool& active) { eventLoopActive = active; }
        inline void setRole(const Role& newRole) { role = newRole; }
    };


    // Visible declarations
    extern Event<ClientID> onClientConnect;
    extern Event<ClientID> onClientDisconnect;
    extern Event<> onHostConnect;
    extern Event<> onHostDisconnect;
    extern bool isInitialized;

    bool init();
    void shutdown();

    bool startDedicatedHost(uint16_t port);
    bool startClient(uint16_t port, const char* hostAddr);

    void clearEventCallbacks();

    template<typename T>
    void sendMessageToHost(T& msgObj){
        Message msg{};
        serializeType(msgObj, msg);
        ENetPacket* packet = enet_packet_create(msg.data,
                                                msg.size,
                                                ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);

        packet->freeCallback = deleteMessageData;
        enet_peer_send(BedrockMetadata::getInstance().getEnetPeer(), 0, packet);
        enet_host_flush(BedrockMetadata::getInstance().getEnetHost());
    }
}

#endif //BEDROCK_BEDROCK_H
