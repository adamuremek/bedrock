#ifndef TESTS_BEHAVIORS_H
#define TESTS_BEHAVIORS_H

#include "ipc.h"
#include <iostream>
#include <cstdlib>

void clientBehavior1(){
    std::string cmd = "ping -c 1 10.0.0.2 > /dev/null 2>&1";
    int result = system(cmd.c_str());

    IPC::sleep(2000);

    if(result == 0){
        std::cout << "Yay :)" << std::endl;
        IPC::addTestResult("Pinged client2", true);
    }else{
        std::cout << "NO :(" << std::endl;
        IPC::addTestResult("Pinged client2", false);
    }
}

void clientBehavior2(){
    std::string cmd = "ping -c 1 10.0.0.1 > /dev/null 2>&1";
    int result = system(cmd.c_str());

    IPC::sleep(2000);

    if(result == 0){
        std::cout << "Yayyyyyy :)" << std::endl;
        IPC::addTestResult("Pinged client1", true);
    }else{
        std::cout << "NOooooooo :(" << std::endl;
        IPC::addTestResult("Pinged client1", false);
    }
}


void registerBehaviors(){
    IPC::registerBehavior("clientBehavior1", clientBehavior1);
    IPC::registerBehavior("clientBehavior2", clientBehavior2);
}


#endif //TESTS_BEHAVIORS_H
