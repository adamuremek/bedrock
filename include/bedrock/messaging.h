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

    void executeCallback(ByteStream stream){
        uint32_t id = 0;
        std::memcpy(&id, stream, sizeof(uint32_t));

        if(callbacks.find(id) != callbacks.end()){
            callbacks[id](stream);
        }
    }
};


#endif //BEDROCK_MESSAGING_H
