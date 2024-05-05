#ifndef BEDROCK_MESSAGING_H
#define BEDROCK_MESSAGING_H
#include "serialization.h"
#include <iostream>
namespace Bedrock{
    class MessageCallbackRegistry{
    private:
        std::unordered_map<uint32_t,  std::function<void(const Message&, Message&)>> callbacks;
    public:
        static BEDROCK_API MessageCallbackRegistry& singleton(){
            static MessageCallbackRegistry callbackRegistry;
            return callbackRegistry;
        }

        template<typename T>
        void registerCallback(std::function<void(T&, Message&)> callback){
            if(!std::is_base_of<BedrockDataType<T>, T>::value){
                return; // Ensure T is derived from BedrockDataType
            }

            callbacks[T::dataId] = [callback](const Message& incomingMsg, Message& outgoingMsg){
                T t{};

                deserializeMessage(incomingMsg, t);
                callback(t, outgoingMsg);
            };
        }

        void executeCallback(const Message& incomingMsg, Message& outgoingMsg){
            uint32_t id = 0;
            std::memcpy(&id, incomingMsg.data, sizeof(uint32_t));

            if(callbacks.find(id) != callbacks.end()){
                callbacks[id](incomingMsg, outgoingMsg);
            }
        }

        void clearCallbacks(){
            callbacks.clear();
        }
    };

    template<typename MessageType>
    inline void BEDROCK_API registerMsgCallback(void (*func)(MessageType&, Message&)){
        MessageCallbackRegistry::singleton().registerCallback<MessageType>(func);
    }

    inline void BEDROCK_API executeMsgCallback(const Message& incomingMsg, Message& outgoingMsg){
        MessageCallbackRegistry::singleton().executeCallback(incomingMsg, outgoingMsg);
    }

    inline void BEDROCK_API clearMsgCallbacks(){
        MessageCallbackRegistry::singleton().clearCallbacks();
    }
}

#endif //BEDROCK_MESSAGING_H
