#include <gtest/gtest.h>
#include <bedrock/bedrock.h>
#include "synchronization.h"
#include "serializers.h"
#include <iostream>

class Client : public ::testing::Test {
public:
    static void onConnect1(){
        // Create float to send
        float f = 3.1415;

        // Send the float
        Bedrock::sendMessageToHost(&f);
        std::cout <<"Sent data to host :)" << std::endl;
    }

protected:
    void SetUp() override {

    }

    void TearDown() override {
        Bedrock::clearEventCallbacks();
    }
};

TEST_F(Client, NormalConnection){
    GTEST_SKIP();
    clientRendezvous();

    // Wait for server to start up
    clientWaitForSignal();

    EXPECT_TRUE(Bedrock::init());
    EXPECT_TRUE(Bedrock::startClient());

    // Provide some time for the client to exist before shutting down
    wait(2000);

    EXPECT_TRUE(Bedrock::stopClient());

    EXPECT_TRUE(Bedrock::shutdown());

    // Give server some time to receive the client disconnect message
    wait(3000);

    // Signal server to start shutdown procedure
    clientSendSignal();
}

TEST_F(Client, AbruptClientDisconnect){
    GTEST_SKIP();
    clientRendezvous();

    // Wait for server to start up
    clientWaitForSignal();

    EXPECT_TRUE(Bedrock::init());
    EXPECT_TRUE(Bedrock::startClient());

    // Provide some time for the client to exist before shutting down
    wait(1000);

    // Kill client abruptly
    std::cout << "KILLING THIS CLIENT" << std::endl;
    SUCCEED();
    return;
}

TEST_F(Client, SendMessageToHost){
    clientRendezvous();

    // Wait for server to start up
    clientWaitForSignal();

    // Register serializers for use
    REGISTER_SERIALIZER(float, serializeFloat);
    REGISTER_DESERIALIZER(float, deserializeFloat);

    // Create hook that will send a message on connect
    Bedrock::clientConnectedToHost += onConnect1;

    // Start the client
    Bedrock::init();
    Bedrock::startClient();

    // Wait a bit for packet to send
    wait(5000);

    // Stop the client
    Bedrock::stopClient();
    Bedrock::shutdown();

    // Signal server to start shutdown procedure
    clientSendSignal();
}

