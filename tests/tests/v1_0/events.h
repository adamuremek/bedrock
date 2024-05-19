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
    EventTestClass2 cls;

    Bedrock::EventCallback<int, int, int*> callback(&cls, &EventTestClass2::func4);
    int counterA = 5;
    int res = callback(3, &counterA);

    ASSERT_EQ(res, 5);
    ASSERT_EQ(counterA, 8);
}

/*========================================================*/

TEST(EventCallbacks, SingleInheritance){
    EventTestClass1* c1 = new EventTestClass3;
    EventTestClass1* c2 = new EventTestClass1;
    EventTestClass3* c3 = new EventTestClass3;

    Bedrock::EventCallback<int> callback1(c1, &EventTestClass1::vfunc1);
    Bedrock::EventCallback<int> callback2(c2, &EventTestClass1::vfunc1);
    Bedrock::EventCallback<int> callback3(c3, &EventTestClass3::vfunc1);

    ASSERT_EQ(callback1(), 10);
    ASSERT_EQ(callback2(), 5);
    ASSERT_EQ(callback3(), 10);

    delete c1;
}

/*========================================================*/

TEST(Events, SubscribeToEventNoReturnNoArgs){
    Bedrock::EventCallback<void> callback([]()->void{
        std::cout << "Hello Events!" << std::endl;
    });


    Bedrock::Event<void> e;
    e.subscribe(callback);
    e.subscribe(Bedrock::EventCallback<void>([]()->void{
        std::cout << "Hello Again!" << std::endl;
    }));

    e.invoke();
    ASSERT_EQ(e.count(), 2);
    e.clear();
    ASSERT_EQ(e.count(), 0);
}

TEST(Events, UnsubscribeFromEventNoReturnNoArgs){
    Bedrock::EventCallback<void> callback([]()->void{
        std::cout << "Hello Events!" << std::endl;
    });


    Bedrock::Event<void> e;
    e.subscribe(callback);
    e.subscribe(Bedrock::EventCallback<void>([]()->void{
        std::cout << "Hello Again!" << std::endl;
    }));


    e.invoke();
    ASSERT_EQ(e.count(), 2);
    e.unsubscribe(callback);
    ASSERT_EQ(e.count(), 1);
    e.invoke();
}

TEST(Events, SubscribeToEventWithReturnNoArgs){
    Bedrock::EventCallback<int> callback1([]()->int{
       return 2;
    });

    Bedrock::EventCallback<int> callback2([]()->int{
        return 4;
    });

    Bedrock::Event<int> e;
    e.subscribe(callback1);
    e.subscribe(callback2);

    ASSERT_EQ(e.count(), 2);
    e.invoke();
    e.clear();
    ASSERT_EQ(e.count(), 0);
}

TEST(Events, UnsubscribeToEventWithReturnNoArgs){
    Bedrock::EventCallback<int> callback1([]()->int{
        return 2;
    });

    Bedrock::EventCallback<int> callback2([]()->int{
        return 4;
    });

    Bedrock::Event<int> e;
    e.subscribe(callback1);
    e.subscribe(callback2);

    ASSERT_EQ(e.count(), 2);
    e.invoke();
    e.unsubscribe(callback2);
    ASSERT_EQ(e.count(), 1);
}

TEST(Events, ahh){
    EventTestClass4* t = new EventTestClass4;

    Bedrock::EventCallback<void, const int&> callback(t, &EventTestClass4::testMap);

    Bedrock::Event<void, const int&> e;

    e.subscribe(callback);
    e.invoke(4);
}


#endif //TESTS_EVENTS_H
