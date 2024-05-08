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

class EventTestingClass{
public:
    void incrementInt(int& val){
        val++;
    }

    void printSmily(){
        std::cout << ":)" << std::endl;
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

/*=====================EVENT TESTING DEFINITIONS=====================*/
void func1(){
    std::cout << "Works :)" << std::endl;
}

int func2(){
    return 5;
}

void func3(int& a, int& b){
    a += 2;
    b += 4;
}

int func4(int& a){
    int oldA = a;
    a +=5;
    return oldA;
}

class EventTestClass1{
public:
    void func1(){
        std::cout << "Hello Test!" << std::endl;
    }

    void func2(int& a, int& b, int c){
        a += c;
        b += c;
    }

    virtual int vfunc1(){
        return 5;
    }
};

class EventTestClass2{
public:
    int func3(){
        return 10;
    }

    int func4(int a, int* b){
        int old = *b;
        *b += a;
        return old;
    }
};

class EventTestClass3 : public EventTestClass1{
public:
    int vfunc1() override{
        return 10;
    }
};

/*===================================================================*/


#endif //TESTS_MSG_TYPES_AND_CALLBACKS_H
