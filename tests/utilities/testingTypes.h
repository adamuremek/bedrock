#ifndef TESTS_SERIALIZERS_H
#define TESTS_SERIALIZERS_H

#include "bedrock/bedrock.h"

BedrockMsgDatatype(Greeting){
    int a = 0;
    int b = 0;
    int c = 0;

    Greeting(){
        registerMember(&a);
        registerMember(&b);
        registerMember(&c);
    }
};

#endif //TESTS_SERIALIZERS_H
