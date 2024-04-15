#include <iostream>
#include "includes.h"
#include <v1_0_tests.h>

int main(int argc, char **argv){
    // Hook behavior functions into the registry
    registerBehaviors();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}