#include <bedrock/bedrock.h>
#include <gtest/gtest.h>
#include <iostream>

BedrockMsgDatatype(TestType1){
    int int1 = 0;
    float float1 = 0.0f;

    TestType1(){
        registerMember(&int1);
        registerMember(&float1);
    }
};

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
    serializeType(t1, incomingMsg);

    // Simulate handling an incoming message
    Bedrock::executeMsgCallback(incomingMsg, outgoingMsg);

    ASSERT_TRUE(outgoingMsg.data != nullptr);

    // Simulate receiving the outgoing message
    TestType1 t2{};
    deserializeMessage(outgoingMsg, t2);


    ASSERT_EQ(t2.int1, 42069);
    ASSERT_EQ(t2.float1, 3.14159f);
}