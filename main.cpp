#include <iostream>
#include <vector>
#include <functional>
#include <enet/enet.h>
#include <bedrock/bedrock.h>
#include <any>

namespace outer{
    namespace{

    }
    static int* a;

    void printAddr(){
        std::cout << &a << std::endl;
    }

}



int main(){
    pid_t pid;
    pid = fork();

    outer::printAddr();

}

