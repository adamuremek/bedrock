#include <iostream>
#include <enet/enet.h>
#include <vector>
#include <cstdio>

struct Message{
    uint8_t* p_data = nullptr;
    size_t p_size = 0;
    void release(){
        delete p_data;
        p_data = nullptr;
        p_size = 0;
    }
};

class Serializer{

};

class Deserializer{

};

template<typename T>
void serialize(T in){
    std::cout << "IS GENERIC" << std::endl;
}

#define SERIALIZE template<> void serialize

template<typename T>
Message createMessage(const T* data, size_t size){
    const uint8_t* byteData = reinterpret_cast<const uint8_t*>(data);

    Message mssg;
    mssg.p_data = new uint8_t[size];
    mssg.p_size = size;

    std::memcpy(mssg.p_data, byteData, size);

    return mssg;
}

//SERIALIZE<int>(int in){
//    std::cout << "IS INT" << std::endl;
//}
//
SERIALIZE<float>(float in){
    std::cout << "IS FLOAT" << std::endl;
}


int main(){
    int a = 5;
    float b = 2.0f;

    serialize(a);
    serialize(b);

}




