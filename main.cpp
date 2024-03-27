#include <iostream>
#include <vector>
#include <functional>
#include <enet/enet.h>
#include <bedrock/bedrock.h>
#include <any>


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

template<typename T>
inline void registerCallback(void (*func)(T)){
    MessageCallbackRegistry::singleton().registerCallback<T>(func);
}

inline void executeCallback(ByteStream stream){
    MessageCallbackRegistry::singleton().executeCallback(stream);
}

template<typename T>
ByteStream serializeType(T& obj){
    if(!std::is_base_of<BedrockDataType<T>, T>::value){
        return nullptr;
    }

    auto& objRef = static_cast<BedrockDataType<T>&>(obj);
    auto out = new Byte[sizeof(uint32_t) + sizeof(T)];
    std::memcpy(out, &T::dataId, sizeof(uint32_t));
    std::memcpy(out + sizeof(uint32_t), &obj, sizeof(T));

    return out;
}

int main(){
    Test1 t{};
    t.a = 10;
    t.b = 69;

    ByteStream b = serializeType(t);

    registerCallback(test);
    executeCallback(b);

   //MessageCallbackRegistry::singleton().registerCallback<Test1>(test);
   // MessageCallbackRegistry::singleton().executeCallback(b);
}

