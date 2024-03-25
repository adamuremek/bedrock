#ifndef TESTS_SERIALIZERS_H
#define TESTS_SERIALIZERS_H

#include <bedrock/bedrock.h>

Message serializeFloat(void* in){
    float* data = static_cast<float*>(in);
    Message mssg;

    mssg.p_data = new Byte[sizeof(float)];
    mssg.p_size = sizeof(float);

    std::memcpy(mssg.p_data, data, mssg.p_size);

    return mssg;
}

void deserializeFloat(Message in, void* out){
    float* outData = static_cast<float*>(out);

    std::memcpy(outData, in.p_data, in.p_size);
}

#endif //TESTS_SERIALIZERS_H
