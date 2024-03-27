#ifndef BEDROCK_BEDROCK_H
#define BEDROCK_BEDROCK_H

#include "types.h"
#include "serialization.h"
#include "event.h"
#include "messaging.h"
#include <enet/enet.h>
#include <thread>



class Bedrock{
private:
    inline static ENetHost* host = nullptr;
    inline static ENetPeer* peer = nullptr;
    inline static std::thread eventLoop;
    inline static bool eventLoopActive = false;

    static void pollClientEvents();
    static void pollHostEvents();

    inline static void deleteMessageData(ENetPacket* packet){
        delete packet->data;
    }
public:
    inline static Event<> clientConnectedToHost;
    inline static Event<> clientDisconnectedFromHost;


    static bool init();
    static bool shutdown();
    static bool startDedicatedHost();
    static bool stopDedicatedHost();
    static bool startClient();
    static bool stopClient();
    static void clearEventCallbacks();

    template<typename T>
    static void sendMessageToHost(T* msgObj){
        Message msg = SERIALIZE<T>(msgObj);
        ENetPacket* packet = enet_packet_create(msg.p_data,
                                                msg.p_size,
                                                ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);

        packet->freeCallback = deleteMessageData;

        enet_peer_send(peer, 0, packet);

        enet_host_flush(host);
    }
};



#endif //BEDROCK_BEDROCK_H
