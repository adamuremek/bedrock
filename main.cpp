#include <iostream>
#include <enet/enet.h>
#include <bedrock/bedrock.h>


template<typename T>
void createMessage(T obj){

}


Message serializeInt(void* in){
    int* data = static_cast<int*>(in);
    Message mssg;

    mssg.p_data = new Byte[sizeof(int)];
    mssg.p_size = sizeof(int);

    std::memcpy(mssg.p_data, data, mssg.p_size);

    return mssg;
}

void deserializeInt(Message in, void* out){
    int* data = static_cast<int*>(out);

    std::memcpy(data, in.p_data, in.p_size);
}

Message serializeFloat(void* in){
    float* data = static_cast<float*>(in);
    Message mssg;

    mssg.p_data = new Byte[sizeof(float)];
    mssg.p_size = sizeof(float);

    std::memcpy(mssg.p_data, data, mssg.p_size);

    return mssg;
}

int main(){
    int a = 5;
    float b = 4.123124176f;

    REGISTER_SERIALIZER(int, serializeInt);
    REGISTER_SERIALIZER(float, serializeFloat);
    REGISTER_DESERIALIZER(int, deserializeInt);

    Message m = SERIALIZE(&a);
    int a2 = DESERIALIZE<int>(m);

    std::cout << a2 << std::endl;
}





