#include <gtest/gtest.h>
#include <bedrock/bedrock.h>
#include "utilities/synchronization.h"
#include "utilities/testingTypes.h"
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
    BedrockConnetion conn;
    conn.port = 8000;
    conn.address = "127.0.0.1";
    EXPECT_TRUE(Bedrock::startClient(conn));

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
    BedrockConnetion conn;
    conn.port = 8000;
    conn.address = "127.0.0.1";
    EXPECT_TRUE(Bedrock::startClient(conn));

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
    BedrockConnetion conn;
    conn.port = 8000;
    conn.address = "127.0.0.1";
    Bedrock::startClient(conn);

    // Wait a bit for packet to send
    wait(5000);

    // Stop the client
    Bedrock::stopClient();
    Bedrock::shutdown();

    // Signal server to start shutdown procedure
    clientSendSignal();
}

