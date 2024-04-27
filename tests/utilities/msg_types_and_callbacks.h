#ifndef TESTS_MSG_TYPES_AND_CALLBACKS_H
#define TESTS_MSG_TYPES_AND_CALLBACKS_H

BedrockMsgDatatype(Numbers){
    int a = 0;

    Numbers(){
        registerMember(&a);
    }
};
BedrockMsgDatatype(TestType1){
    int int1 = 0;
    float float1 = 0.0f;

    TestType1(){
        registerMember(&int1);
        registerMember(&float1);
    }
};

BedrockMsgDatatype(TestType2){
    int a{0};
    int b{0};
    int c{0};

    TestType2(){
        registerMember(&a);
        registerMember(&b);
        registerMember(&c);
    };
};

BedrockMsgDatatype(TestType3){
    int a{0};
    TestType2 t{};

    TestType3(){
        registerMember(&a);
        registerBedrockMsgDatatypeMember(&t);
    }
};

void ModifyNum1(Numbers& inNum, Bedrock::Message& outMsg){
    inNum.a = 37;

    Bedrock::serializeType(inNum, outMsg);
}

void ModifyNum2(Numbers& inNum, Bedrock::Message& outMsg){
    inNum.a += 1;

    Bedrock::serializeType(inNum, outMsg);
}

void PrintNum(Numbers& inNum, Bedrock::Message& outMsg){
    std::cout << inNum.a << std::endl;
}

void HandleTestType3(TestType3& inType, Bedrock::Message& outMsg){
    inType.a = 5;
    inType.t.a = 6;
    inType.t.b = 7;
    inType.t.c = 8;

    Bedrock::serializeType(inType, outMsg);
}


#endif //TESTS_MSG_TYPES_AND_CALLBACKS_H
