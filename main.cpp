#include <iostream>
#include <vector>
#include <functional>
#include <enet/enet.h>
#include <bedrock/bedrock.h>
#include <any>

struct test{
    int a = 0;

    test(){
        std::cout << &a << std::endl;
    }
};


int main(){
    test t{};
    std::cout << &t.a << std::endl;
}

