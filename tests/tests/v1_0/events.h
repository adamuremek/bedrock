#ifndef TESTS_EVENTS_H
#define TESTS_EVENTS_H

TEST(Events, CreateAndUseEventNoArgs){
    Bedrock::Event<> e{};

    int counter = 0;

    e += [&counter]() -> void {
        counter += 10;
    };


    e += [&counter]() -> void {
        counter += 15;
    };

    e.invoke();

    ASSERT_EQ(counter, 25);
}

TEST(Events, CreateAndUSeEventWithArgs){
    Bedrock::Event<int, int, int> e{};

    int counter = 0;

    e += [&counter](int int1, int int2, int int3) -> void {
        counter += int1 + int2 + int3;
    };

    e += [&counter](int int1, int int2, int int3) -> void {
        counter += int1 * int2 * int3;
    };

    e.invoke(2, 4, 6);

    ASSERT_EQ(counter, 60);
}

TEST(Events, CountAndClear){
    Bedrock::Event<> e{};

    int counter = 0;

    e += [&counter]() -> void {
        counter += 1;
    };


    e += [&counter]() -> void {
        counter += 1;
    };

    e.invoke();

    ASSERT_EQ(counter, 2);
    ASSERT_EQ(e.count(), 2);

    e.clear();

    ASSERT_EQ(e.count(), 0);
}


#endif //TESTS_EVENTS_H
