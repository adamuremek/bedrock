#ifndef TESTS_MSG_TYPES_AND_CALLBACKS_H
#define TESTS_MSG_TYPES_AND_CALLBACKS_H

BedrockMsgDatatype(Numbers){
    int a = 0;

    Numbers(){
        registerMember(&a);
    }
};

void ModifyNum1(Numbers& inNum, Bedrock::Message& outMsg){
    inNum.a = 37;

    Bedrock::serializeType(inNum, outMsg);
}

void PrintNum(Numbers& inNum, Bedrock::Message& outMsg){
    std::cout << inNum.a << std::endl;
}


#endif //TESTS_MSG_TYPES_AND_CALLBACKS_H
