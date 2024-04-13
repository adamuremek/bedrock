#ifndef BEDROCK_SERIALIZATION_H
#define BEDROCK_SERIALIZATION_H

#include "types.h"
#include <typeindex>
#include <utility>
#include <cstring>

namespace Bedrock{
    template<typename T>
    void serializeType(T& obj, Message& outMsg){
        if(!std::is_base_of<BedrockDataType<T>, T>::value){
            return;
        }

        auto& objRef = static_cast<BedrockDataType<T>&>(obj);
        auto outBuffer = new Byte[sizeof(uint32_t) + objRef.size];

        // Copy the data type ID to the buffer
        std::memcpy(outBuffer, &T::dataId, sizeof(uint32_t));

        // Copy the member data to the buffer
        size_t offset = sizeof(uint32_t);
        for(const auto& pair : objRef.members){
            std::memcpy(outBuffer + offset, pair.first, pair.second);
            offset += pair.second;
        }


        outMsg.data = outBuffer;
        outMsg.size = sizeof(uint32_t) + objRef.size;
    }

    template<typename T>
    void deserializeMessage(const Message& inMsg, T& outObj){
        if(!std::is_base_of<BedrockDataType<T>, T>::value){
            return;
        }

        auto& objRef = static_cast<BedrockDataType<T>&>(outObj);

        // TODO: check to make sure message id is equal to data type id

        size_t offset = sizeof(uint32_t);
        for(const auto& pair : objRef.members){
            std::memcpy(pair.first, inMsg.data + offset, pair.second);
            offset += pair.second;
        }
    }
}



#endif //BEDROCK_SERIALIZATION_H
