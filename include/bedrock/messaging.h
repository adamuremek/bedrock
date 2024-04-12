#ifndef BEDROCK_MESSAGING_H
#define BEDROCK_MESSAGING_H
#include "serialization.h"

namespace Bedrock{
    class MessageCallbackRegistry{
    private:
        std::unordered_map<uint32_t,  std::function<void(const Message&, Message&)>> callbacks;
    public:
        static MessageCallbackRegistry& singleton(){
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
    };

    template<typename MessageType>
    inline void registerMsgCallback(void (*func)(MessageType&, Message&)){
        MessageCallbackRegistry::singleton().registerCallback<MessageType>(func);
    }

    inline void executeMsgCallback(const Message& incomingMsg, Message& outgoingMsg){
        MessageCallbackRegistry::singleton().executeCallback(incomingMsg, outgoingMsg);
    }
}

#endif //BEDROCK_MESSAGING_H
