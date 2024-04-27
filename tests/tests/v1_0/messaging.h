#ifndef TESTS_MESSAGING_H
#define TESTS_MESSAGING_H



void callback1(TestType1& t, Bedrock::Message& outMsg){
    std::cout << "'int1' value is: " << t.int1<< std::endl;
}

void callback2(TestType1& t, Bedrock::Message& outMsg){
    t.int1 = 42069;
    t.float1 = 3.14159f;


    serializeType(t, outMsg);
}


TEST(Messages, SerializeCustomType){
    TestType1 t1{};
    Bedrock::Message msg{};

    serializeType(t1, msg);

    ASSERT_TRUE(msg.size == 12);
}

TEST(Messages, DeserializeCustomType){
    TestType1 t1{};
    TestType1 t2{};
    Bedrock::Message msg{};

    t1.int1 = 5;
    t1.float1 = 3.14159f;

    serializeType(t1, msg);
    deserializeMessage(msg, t2);

    ASSERT_EQ(t2.int1, 5);
    ASSERT_EQ(t1.float1, 3.14159f);
}


TEST(MessagingCallbacks, RegisterCallback){
    Bedrock::registerMsgCallback(callback1);
}

TEST(MessagingCallbacks, ExecuteCallbackNoOutgoingMsg){
    Bedrock::registerMsgCallback(callback1);

    TestType1 t1{};
    t1.int1 = 5;

    // Simulate an incoming message
    Bedrock::Message incomingMsg{};
    serializeType(t1, incomingMsg);

    // Simulate handling an incoming message
    Bedrock::Message outgoingMsg{};

    Bedrock::executeMsgCallback(incomingMsg, outgoingMsg);

    ASSERT_TRUE(outgoingMsg.data == nullptr);
}

TEST(MessagingCallbacks, ExecuteCallbackWithOutgoingMsg){
    Bedrock::registerMsgCallback(callback2);

    TestType1 t1{};
    t1.int1 = 5;

    Bedrock::Message outgoingMsg{};


    // Simulate an incoming message
    Bedrock::Message incomingMsg{};
    Bedrock::serializeType(t1, incomingMsg);

    // Simulate handling an incoming message
    Bedrock::executeMsgCallback(incomingMsg, outgoingMsg);

    ASSERT_TRUE(outgoingMsg.data != nullptr);

    // Simulate receiving the outgoing message
    TestType1 t2{};
    Bedrock::deserializeMessage(outgoingMsg, t2);


    ASSERT_EQ(t2.int1, 42069);
    ASSERT_EQ(t2.float1, 3.14159f);
}

TEST(MessagingCallbacks, NestedMessageTypes){
    TestType3 t{};
    t.a = 1;
    t.t.a = 2;
    t.t.b = 3;
    t.t.c = 4;

    Bedrock::Message outMsg{};

    Bedrock::serializeType(t, outMsg);

    TestType3 t2{};
    Bedrock::deserializeMessage(outMsg, t2);

    ASSERT_EQ(t2.a, 1);
    ASSERT_EQ(t2.t.a, 2);
    ASSERT_EQ(t2.t.b, 3);
    ASSERT_EQ(t2.t.c, 4);

    delete outMsg.data;
}

TEST(MessagingCallbacks, NestedMessageTypesWithCallbacks){
    Bedrock::registerMsgCallback(HandleTestType3);

    TestType3 t{};
    t.a = 1;
    t.t.a = 2;
    t.t.b = 3;
    t.t.c = 4;

    Bedrock::Message inMsg{};
    Bedrock::Message outMsg{};

    Bedrock::serializeType(t, inMsg);

    Bedrock::executeMsgCallback(inMsg, outMsg);

    TestType3 t2{};
    Bedrock::deserializeMessage(outMsg, t2);

    ASSERT_EQ(t2.a, 5);
    ASSERT_EQ(t2.t.a, 6);
    ASSERT_EQ(t2.t.b, 7);
    ASSERT_EQ(t2.t.c, 8);

    delete outMsg.data;
}

#endif //TESTS_MESSAGING_H
