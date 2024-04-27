#ifndef BEDROCK_TYPES_H
#define BEDROCK_TYPES_H

#include <functional>
#include <cstdint>
#include <string>
#include <enet/enet.h>
#include <type_traits>
#include <vector>

namespace Bedrock{
    using ByteStream = unsigned char*;
    using Byte = unsigned char;
    using StreamSize = unsigned long;
    using MemberInfo = std::pair<void*, std::size_t>;
    using ClientID = int;
    using LayerId = uint32_t;


    enum Role : unsigned char{
        ACTOR_NONE,
        ACTOR_SERVER,
        ACTOR_CLIENT
    };

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
        std::vector<MemberInfo> members;
        size_t size = 0;

        struct Registrar{
            inline static uint32_t curId = 0;
            explicit Registrar(uint32_t& id){
                id = curId;
                curId++;
            }
        };

        template<typename T>
        void registerMember(T* member){
            static_assert(!std::is_pointer<T>::value, "Pointer members are not allowed!");
            members.emplace_back(static_cast<void*>(member), sizeof(T));
            size += sizeof(T);
        }

    };

    template<typename T>
    struct BedrockDataType : public BedrockDataBase{
        inline static uint32_t dataId;
        inline static Registrar _reg{dataId};
    };
}

#define BedrockMsgDatatype(Name) \
    struct Name : public Bedrock::BedrockDataType<Name>


#endif //BEDROCK_TYPES_H
