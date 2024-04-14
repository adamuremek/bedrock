#ifndef TESTS_BEHAVIORS_H
#define TESTS_BEHAVIORS_H

#include "ipc.h"
#include <iostream>

void clientBehavior1(){
    std::cout << "penis balls" << std::endl;
}

void clientBehavior2(){
    std::cout << "fuck shit" << std::endl;
}


void registerBehaviors(){
    IPC::registerBehavior("clientBehavior1", clientBehavior1);
    IPC::registerBehavior("clientBehavior2", clientBehavior2);
}


#endif //TESTS_BEHAVIORS_H
