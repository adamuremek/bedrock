#ifndef BEDROCK_BEDROCK_H
#define BEDROCK_BEDROCK_H

#include "types.h"
#include "serialization.h"
#include "event.h"
#include "messaging.h"
#include <enet/enet.h>
#include <thread>

#define ACTOR_NONE static_cast<unsigned char>(0)
#define ACTOR_CLIENT static_cast<unsigned char>(1)
#define ACTOR_SERVER static_cast<unsigned char>(2)


namespace Bedrock{
    // Non-visible declarations (outside of Bedrock)
    namespace{
        ENetHost* enetHost = nullptr;
        ENetPeer* enetPeer = nullptr;
        std::thread eventLoop;
        bool eventLoopActive = false;

        unsigned char actingAs = ACTOR_NONE;

        inline void deleteMessageData(ENetPacket* packet){
            delete packet->data;
        }
    }


    // Visible declarations
    extern Event<ClientID> onClientConnect;
    extern Event<ClientID> onClientDisconnect;
    extern Event<> onHostConnect;
    extern Event<> onHostDisconnect;
    extern bool isInitialized;

    bool init();
    void shutdown();

    bool startDedicatedHost(uint16_t port);
    bool startClient(uint16_t port, const char* hostId);

    void clearEventCallbacks();

    template<typename T>
    void sendMessageToHost(T& msgObj){
        Message msg = serializeType(msgObj);
        ENetPacket* packet = enet_packet_create(msg.data,
                                                msg.size,
                                                ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);

        packet->freeCallback = deleteMessageData;

        enet_peer_send(enetPeer, 0, packet);

        enet_host_flush(enetHost);
    }
}

#endif //BEDROCK_BEDROCK_H
