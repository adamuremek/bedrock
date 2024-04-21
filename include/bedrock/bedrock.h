#ifndef BEDROCK_BEDROCK_H
#define BEDROCK_BEDROCK_H

#include "types.h"
#include "layers.h"
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

        inline bool isRole(const Role& roleQuery) const { return roleQuery == role; }

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

    // Non-visible declarations (outside of Bedrock)
    namespace{
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
    bool startClient(uint16_t port, const char* hostAddr);

    void clearEventCallbacks();


    void sendMessageToHost(const Message& msg);

    template<typename T>
    void sendToHost(T& msgObj){
        Message msg{};
        serializeType(msgObj, msg);

        sendMessageToHost(msg);
    }

    void sendMessageToClient(const Message& msg, ClientID client);

    template<typename T>
    void sendToClient(T& msgObj, ClientID client){
        Message msg{};
        serializeType(msgObj, msg);

        sendMessageToClient(msg, client);
    }

    void sendMessageToLayer(const Message& msg, LayerId layer);

    template<typename T>
    void sendToLayer(T& msgObj, LayerId layer){
        Message msg{};
        serializeType(msgObj, msg);

        sendMessageToLayer(msg, layer);
    }



}

#endif //BEDROCK_BEDROCK_H
