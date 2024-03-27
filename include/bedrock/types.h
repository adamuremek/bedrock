#ifndef BEDROCK_TYPES_H
#define BEDROCK_TYPES_H

#include <functional>
#include <cstdint>

using ByteStream = unsigned char*;
using Byte = unsigned char;
using StreamSize = unsigned long;

struct Message{
    ByteStream data = nullptr;
    StreamSize size = 0;
    void release(){
        delete data;
        data = nullptr;
        size = 0;
    }
};

struct BedrockDataBase{
    struct Registrar{
        inline static uint32_t curId = 0;
        explicit Registrar(uint32_t& id){
            id = curId;
            curId++;
        }
    };
};

template<typename T>
struct BedrockDataType : public BedrockDataBase{
    inline static uint32_t dataId;
    inline static Registrar _reg{dataId};
};


using SerializeFunc = std::function<Message(void*)>;
using DeserializeFunc = std::function<void(Message, void*)>;


#define BEDROCK_DATA_TYPE(Name) \
    struct Name : public BedrockDataType<Name>


#endif //BEDROCK_TYPES_H
