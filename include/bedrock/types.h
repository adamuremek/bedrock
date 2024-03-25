#ifndef BEDROCK_TYPES_H
#define BEDROCK_TYPES_H

#include <functional>

using ByteStream = unsigned char*;
using Byte = unsigned char;
using StreamSize = unsigned long;

struct Message{
    ByteStream p_data = nullptr;
    StreamSize p_size = 0;
    void release(){
        delete p_data;
        p_data = nullptr;
        p_size = 0;
    }
};

using SerializeFunc = std::function<Message(void*)>;
using DeserializeFunc = std::function<void(Message, void*)>;




#endif //BEDROCK_TYPES_H
