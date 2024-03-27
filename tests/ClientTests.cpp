#include <gtest/gtest.h>
#include <bedrock/bedrock.h>
#include "synchronization.h"
#include "testingTypes.h"
#include <iostream>

class Client : public ::testing::Test {
protected:
    void SetUp() override {

    }

    void TearDown() override {
        Bedrock::clearEventCallbacks();
    }
public:
    static void onConnect1(){
        //Create greeting to send
        Greeting greet{};
        greet.a = 10;
        greet.b = 20;
        greet.c = 30;

        // Send the float
        Bedrock::sendMessageToHost(greet);
        std::cout <<"Sent data to host :)" << std::endl;
    }

    static void receiveGreeting(Greeting greet){
        EXPECT_EQ(greet.a, 5);
        EXPECT_EQ(greet.b, 15);
        EXPECT_EQ(greet.c, 25);
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

