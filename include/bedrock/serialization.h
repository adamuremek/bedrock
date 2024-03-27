#ifndef BEDROCK_SERIALIZATION_H
#define BEDROCK_SERIALIZATION_H

#include "types.h"
#include <typeindex>

class SerializationRegistry {
private:
    std::unordered_map<std::type_index, SerializeFunc> serializers;
    std::unordered_map<std::type_index, DeserializeFunc> deserializers;
public:
    static SerializationRegistry &getInstance() {
        static SerializationRegistry instance;
        return instance;
    }

    template<typename T>
    void registerSerializer(SerializeFunc serializer) {
        serializers[typeid(T)] = std::move(serializer);
    }

    template<typename T>
    void registerDeserializer(DeserializeFunc deserializer) {
        deserializers[typeid(T)] = std::move(deserializer);
    }

    template<typename T>
    Message serialize(T *obj) {
        Message mssg;

        auto it = serializers.find(typeid(T));
        if (it != serializers.end()) {
            mssg = it->second(static_cast<void *>(obj));
        } else {
            //std::cout << "No serializer registered for this type." << std::endl;
        }

        return mssg;
    }

    template<typename T>
    T deserialize(Message mssg) {
        T res{};
        auto it = deserializers.find(typeid(T));
        if (it != deserializers.end()) {
            it->second(mssg, &res);
        } else {
            //std::cout << "No serializer registered for this type." << std::endl;
        }

        return res;
    }
};



#define REGISTER_SERIALIZER(T, func) \
    SerializationRegistry::getInstance().registerSerializer<T>(func)

#define REGISTER_DESERIALIZER(T, func) \
    SerializationRegistry::getInstance().registerDeserializer<T>(func)

#define SERIALIZE \
    SerializationRegistry::getInstance().serialize

#define DESERIALIZE \
    SerializationRegistry::getInstance().deserialize

#endif //BEDROCK_SERIALIZATION_H
