#ifndef BEDROCK_MESSAGING_H
#define BEDROCK_MESSAGING_H

class MessageCallbackRegistry{
private:
    std::unordered_map<uint32_t,  std::function<void(ByteStream)>> callbacks;
public:
    static MessageCallbackRegistry& singleton(){
        static MessageCallbackRegistry callbackRegistry;
        return callbackRegistry;
    }

    template<typename T>
    void registerCallback(std::function<void(T)> callback){
        if(!std::is_base_of<BedrockDataType<T>, T>::value){
            return; // Ensure T is derived from BedrockDataType
        }

        callbacks[T::dataId] = [callback](ByteStream stream){
            T t{};

            std::memcpy(&t, stream + sizeof(uint32_t ), sizeof(T));

            callback(t);

        };
    }

    void executeCallback(Message msg){
        uint32_t id = 0;
        std::memcpy(&id, msg.data, sizeof(uint32_t));

        if(callbacks.find(id) != callbacks.end()){
            callbacks[id](msg.data);
        }
    }
};

template<typename T>
inline void registerCallback(void (*func)(T)){
    MessageCallbackRegistry::singleton().registerCallback<T>(func);
}

inline void executeCallback(Message msg){
    MessageCallbackRegistry::singleton().executeCallback(msg);
}


#endif //BEDROCK_MESSAGING_H
