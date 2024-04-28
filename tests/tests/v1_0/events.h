#ifndef TESTS_EVENTS_H
#define TESTS_EVENTS_H

TEST(Events, CreateAndUseEventNoArgs){
    Bedrock::Event<> e{};

    int counter = 0;
    auto lamb1 = []() -> void {
        std::cout << "lamb1 ran :)" << std::endl;
        ASSERT_TRUE(true);
    };

    e.subscribe(lamb1);


    auto lamb2 = []() -> void {
        std::cout << "lamb2 ran :)" << std::endl;
        ASSERT_TRUE(true);
    };

    e.subscribe(lamb2);

    e.invoke();

    e.unsubscribe(lamb2);

    e.invoke();
}

TEST(Events, CreateAndUSeEventWithArgs){
    Bedrock::Event<int, int, int&> e{};

    int counter = 0;

    e.subscribe([](int int1, int int2, int& counter) -> void {
        counter += int1 + int2;
    });

    e.subscribe([](int int1, int int2, int& counter) -> void {
        counter += int1 * int2;
    });

    e.invoke(2, 4, counter);

    ASSERT_EQ(counter, 14);
}

TEST(Events, CountAndClear){
    Bedrock::Event<int&> e{};

    int counter = 0;

    e.subscribe([](int& counter) -> void {
        counter += 1;
    });


    e.subscribe([](int& counter) -> void {
        counter += 1;
    });

    e.invoke(counter);

    ASSERT_EQ(counter, 2);
    ASSERT_EQ(e.count(), 2);

    e.clear();

    ASSERT_EQ(e.count(), 0);
}


#endif //TESTS_EVENTS_H
