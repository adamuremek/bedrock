#ifndef BEDROCK_BEDROCK_H
#define BEDROCK_BEDROCK_H

#ifdef _WIN32
    #ifdef BUILD_SHARED_LIB
        #ifdef BEDROCK_DLL
            #define BEDROCK_API __declspec(dllexport)
        #else
            #define BEDROCK_API __declspec(dllimport)
        #endif // endif BEDROCK_DLL
    #else
        #define BEDROCK_API
    #endif
#else
    #define BEDROCK_API
#endif

#include <thread>
#include <iostream>
#include "types.h"
#include "layers.h"
#include "serialization.h"
#include "event.h"
#include "messaging.h"
#include <enet/enet.h>

namespace Bedrock{
    class BedrockMetadata{
    private:
        ENetHost* enetHost = nullptr;
        ENetPeer* enetPeer = nullptr;
        bool eventLoopActive = false;

        Role role = ACTOR_NONE;
    public:
        std::thread eventLoop; //TODO

        static BEDROCK_API BedrockMetadata& getInstance(){
            static BedrockMetadata instance;
            return instance;
        }

        [[nodiscard]] inline bool isRole(const Role& roleQuery) const { return roleQuery == role; }

        // Getters
        [[nodiscard]] inline ENetHost* getEnetHost() const {return enetHost; }
        [[nodiscard]] inline ENetPeer* getEnetPeer() const { return enetPeer; }
        [[nodiscard]] inline bool getEventLoopActive() const { return eventLoopActive; }

        // Setters
        inline void setEnetHost(ENetHost* host) { enetHost = host; }
        inline void setEnetPeer(ENetPeer* peer) { enetPeer = peer; }
        inline void setEventLoopActive(const bool& active) { eventLoopActive = active; }
        inline void setRole(const Role& newRole) { role = newRole; }
    };

    // Non-visible declarations (outside of Bedrock namespace)
    namespace{
        inline void deleteMessageData(ENetPacket* packet){
            delete packet->data;
        }
    }

    // Visible declarations
    extern BEDROCK_API Event<void, const ClientID&> onClientConnect;
    extern BEDROCK_API Event<void, const ClientID&> onClientDisconnect;
    extern BEDROCK_API Event<void> onHostConnect;
    extern BEDROCK_API Event<void> onHostDisconnect;
    extern BEDROCK_API bool isInitialized;

    StatusCode BEDROCK_API init();
    StatusCode BEDROCK_API shutdown();
    bool BEDROCK_API isRole(const Role& roleQuery);

    StatusCode BEDROCK_API startDedicatedHost(uint16_t port);
    StatusCode BEDROCK_API startClient(uint16_t port, const char* hostAddr);

    void BEDROCK_API clearEventCallbacks();


    StatusCode BEDROCK_API sendMessageToHost(const Message& msg);
    StatusCode BEDROCK_API sendMessageToClient(const Message& msg, ClientID client);
    StatusCode BEDROCK_API sendMessageToLayer(const Message& msg, LayerId layer);

    template<typename T>
    void sendToHost(T& msgObj){
        Message msg{};
        serializeType(msgObj, msg);

        sendMessageToHost(msg);
    }

    template<typename T>
    void sendToClient(T& msgObj, ClientID client){
        Message msg{};
        serializeType(msgObj, msg);

        sendMessageToClient(msg, client);
    }

    template<typename T>
    void sendToLayer(T& msgObj, LayerId layer){
        Message msg{};
        serializeType(msgObj, msg);

        sendMessageToLayer(msg, layer);
    }
}

#endif //BEDROCK_BEDROCK_H
