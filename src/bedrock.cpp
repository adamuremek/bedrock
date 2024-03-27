#include "bedrock/bedrock.h"
#include <iostream>


void Bedrock::pollClientEvents() {
    ENetEvent event;

    while (eventLoopActive) {
        enet_host_service(host, &event, 0);

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
                eventLoopActive = false;
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                std::cout << "Message received from host" << std::endl;
                enet_packet_destroy(event.packet);
                break;
            }
        }
    }

    std::cout << "===CLIENT POLL END===" << std::endl;
}

void Bedrock::pollHostEvents() {
    ENetEvent event;

    while (eventLoopActive) {
        enet_host_service(host, &event, 0);

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
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                std::cout << "Message received from Client" << std::endl;
                enet_packet_destroy(event.packet);
                break;
            }
        }
    }

    std::cout << "===HOST POLL END===" << std::endl;
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

bool Bedrock::startDedicatedHost() {
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 7000;

    host = enet_host_create(&address, 32, 2, 0, 0);

    if(host == nullptr){
        std::cout << "ASS"<< std::endl;
    }


    eventLoopActive = true;
    eventLoop = std::thread(Bedrock::pollHostEvents);

    return true;
}

bool Bedrock::stopDedicatedHost() {
    eventLoopActive = false;
    eventLoop.join();
    enet_host_destroy(host);

    return true;
}

bool Bedrock::startClient() {
    host = enet_host_create(nullptr, 1, 2, 0, 0);

    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 7000;

    peer = enet_host_connect(host, &address, 2, 0);

    eventLoopActive = true;
    eventLoop = std::thread(Bedrock::pollClientEvents);

    return true;
}

bool Bedrock::stopClient() {
    enet_peer_disconnect(peer, 0);
    enet_host_flush(host);

    eventLoop.join();

    enet_host_destroy(host);
    return true;
}

void Bedrock::clearEventCallbacks() {
    clientConnectedToHost.clear();
    clientDisconnectedFromHost.clear();
}

//template<typename T>
//void Bedrock::sendMessageToHost(T* msgObj) {
//    Message msg = SERIALIZE<T>(msgObj);
//    ENetPacket* packet = enet_packet_create(msg.p_data,
//                                            msg.p_size,
//                                            ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);
//
//    packet->freeCallback = deleteMessageData;
//
//    enet_peer_send(peer, 0, packet);
//
//    //enet_host_flush(host);
//}

