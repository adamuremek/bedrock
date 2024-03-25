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


int main(){
    Test1 t{};
    t.a = 10;
    t.b = 69;

    ByteStream b = serializeType<Test1>(t);

    MessageCallbackRegistry::getInstance().registerCallback<Test1>(test);
    MessageCallbackRegistry::getInstance().executeCallback(b);
}

