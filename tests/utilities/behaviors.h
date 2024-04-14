#ifndef TESTS_BEHAVIORS_H
#define TESTS_BEHAVIORS_H

#include "ipc.h"
#include <iostream>

void clientBehavior1(){
    std::cout << "My cock is hard" << std::endl;
    IPC::addTestResult("My cock is indeed hard", true);

}

void clientBehavior2(){
    std::cout << "I shat my pants" << std::endl;
    IPC::addTestResult("I did not shat my pants", false);
}


void registerBehaviors(){
    IPC::registerBehavior("clientBehavior1", clientBehavior1);
    IPC::registerBehavior("clientBehavior2", clientBehavior2);
}


#endif //TESTS_BEHAVIORS_H
