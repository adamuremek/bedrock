#ifndef TESTS_EVENTS_H
#define TESTS_EVENTS_H

TEST(EventCallbacks, LambdaCallbackNoReturnNoArgs){
    Bedrock::EventCallback<void> callback([]() -> void{
       std::cout << "Works :)" << std::endl;
    });

    callback();
}

TEST(EventCallbacks, LambdaCallbackWithReturnNoArgs){
    Bedrock::EventCallback<int> callback([]() -> int {
        return 5;
    });

    ASSERT_EQ(callback(), 5);
}

TEST(EventCallbacks, LambdaCallbackNoReturnWithArgs){
    Bedrock::EventCallback<void, int&, int&> callback([](int& a, int& b) -> void {
        a += 2;
        b += 4;
    });

    int counterA = 0, counterB = 0;
    callback(counterA, counterB);

    ASSERT_EQ(counterA, 2);
    ASSERT_EQ(counterB, 4);
}

TEST(EventCallbacks, LambdaCallbackWithReturnWithArgs){
    Bedrock::EventCallback<int, int&> callback([](int& a) -> int {
        int oldA = a;
        a +=5;
        return oldA;
    });

    int counterA = 2;
    int oldA = callback(counterA);

    ASSERT_EQ(counterA, 7);
    ASSERT_EQ(oldA, 2);
}

/*========================================================*/

TEST(EventCallbacks, FunctionPointerCallbackNoReturnNoArgs){
    Bedrock::EventCallback<void> callback(func1);
    callback();
}

TEST(EventCallbacks, FunctionPointerWithReturnNoArgs){
    Bedrock::EventCallback<int> callback(func2);

    ASSERT_EQ(callback(), 5);
}

TEST(EventCallbacks, FunctionPointerNoReturnWithArgs){
    Bedrock::EventCallback<void, int&, int&> callback(func3);

    int counterA = 0, counterB = 0;
    callback(counterA, counterB);

    ASSERT_EQ(counterA, 2);
    ASSERT_EQ(counterB, 4);
}

TEST(EventCallbacks, FunctionPointerWithReturnWithArgs){
    Bedrock::EventCallback<int, int&> callback(func4);

    int counterA = 2;
    int oldA = callback(counterA);

    ASSERT_EQ(counterA, 7);
    ASSERT_EQ(oldA, 2);
}

/*========================================================*/

TEST(EventCallbacks, MemberFunctionPointerCallbackNoReturnNoArgs){
    EventTestClass1 cls;
    Bedrock::EventCallback<void> callback(&cls, &EventTestClass1::func1);

    callback();
}

TEST(EventCallbacks, MemberFunctionPointerWithReturnNoArgs){
    EventTestClass2 cls;
    Bedrock::EventCallback<int> callback(&cls, &EventTestClass2::func3);

    ASSERT_EQ(callback(), 10);
}

TEST(EventCallbacks, MemberFunctionPointerNoReturnWithArgs){
    EventTestClass1 cls;
    Bedrock::EventCallback<void, int&, int&, int> callback(&cls, &EventTestClass1::func2);

    int counterA = 5, counterB = 10;
    callback(counterA, counterB, 5);

    ASSERT_EQ(counterA, 10);
    ASSERT_EQ(counterB, 15);
}

TEST(EventCallbacks, MemberFunctionPointerWithReturnWithArgs){

}

/*========================================================*/

TEST(Events, SubscribeToEventNoArgs){
    Bedrock::EventCallback<void> callback([]()->void{
        std::cout << "Hello Events!" << std::endl;
    });


    Bedrock::Event<void> e;
    e.subscribe(callback);
    e.subscribe(Bedrock::EventCallback<void>([]()->void{
        std::cout << "Hello Again!" << std::endl;
    }));


    e.invoke();
}

TEST(Events, UnsubscribeFromEventNoArgs){
    Bedrock::EventCallback<void> callback([]()->void{
        std::cout << "Hello Events!" << std::endl;
    });


    Bedrock::Event<void> e;
    e.subscribe(callback);
    e.subscribe(Bedrock::EventCallback<void>([]()->void{
        std::cout << "Hello Again!" << std::endl;
    }));


    e.invoke();

    e.unsubscribe(callback);

    e.invoke();
}


#endif //TESTS_EVENTS_H
