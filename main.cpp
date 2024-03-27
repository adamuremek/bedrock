#include <iostream>
#include <vector>
#include <functional>
#include <enet/enet.h>
#include <bedrock/bedrock.h>
#include <any>

struct test{
    int a;
    int* b;
};

int main(){

    std::cout << sizeof(test) << std::endl;
}

