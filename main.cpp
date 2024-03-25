#include <iostream>
#include <vector>
#include <functional>
#include <enet/enet.h>
#include <bedrock/bedrock.h>
#include <any>

struct BedrockDataBase{
    struct Registrar{
        inline static uint32_t curId = 0;
        explicit Registrar(uint32_t& id){
            id = curId;
            curId++;
        }
    };
};

template<typename Cum>
struct BedrockDataType : public BedrockDataBase{
    inline static uint32_t dataId;
    inline static Registrar _reg{dataId};
};

#define BEDROCK_DATA_TYPE(Name) \
    struct Name : public BedrockDataType<Name>




class temp{
private:
    std::unordered_map<uint32_t,  std::function<void(ByteStream)>> callbacks;
public:
    static temp& getInstance(){
        static temp t;
        return t;
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



template<typename T>
ByteStream serializeType(T& obj){
    if(!std::is_base_of<BedrockDataType<T>, T>::value){
        return nullptr;
    }

    auto& objRef = static_cast<BedrockDataType<T>&>(obj);
    ByteStream out = new Byte[sizeof(uint32_t) + sizeof(T)];
    std::memcpy(out, &T::dataId, sizeof(uint32_t));
    std::memcpy(out + sizeof(uint32_t), &obj, sizeof(T));

    return out;
}


BEDROCK_DATA_TYPE(Test1){
    short a;
    int b;
    float c[10];
};



void test(Test1 t){
    std::cout << t.a << std::endl;
    std::cout << t.b << std::endl;
    std::cout << sizeof(t.c) << std::endl;
}

int main(){
    Test1 t{};
    t.a = 10;
    t.b = 69;

    ByteStream b = serializeType<Test1>(t);


    temp::getInstance().registerCallback<Test1>(test);
    temp::getInstance().executeCallback(b);
}

