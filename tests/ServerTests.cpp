#include <gtest/gtest.h>
#include <bedrock/bedrock.h>
#include "synchronization.h"
#include "testingTypes.h"
#include <iostream>

class Server : public ::testing::Test {
private:
    inline static bool CLIENT_CONNECTED_FLAG = false;
    inline static bool CLIENT_DISCONNECTED_FLAG = false;

    static void clientConnect(){
        std::cout << "Client connected to host!" << std::endl;
        CLIENT_CONNECTED_FLAG = true;
    }

    static void clientDisconnect(){
        std::cout << "Client disconnected from host!" << std::endl;
        CLIENT_DISCONNECTED_FLAG = true;
    }

protected:
    static void evaluateFlags(){
        EXPECT_TRUE(CLIENT_CONNECTED_FLAG);
        EXPECT_TRUE(CLIENT_DISCONNECTED_FLAG);
    }

    void SetUp() override {
        Bedrock::clientConnectedToHost += clientConnect;
        Bedrock::clientDisconnectedFromHost += clientDisconnect;
    }

    void TearDown() override {
        Bedrock::clearEventCallbacks();
    }
public:
    static void receiveGreeting(Greeting greet){
        EXPECT_EQ(greet.a, 10);
        EXPECT_EQ(greet.b, 20);
        EXPECT_EQ(greet.c, 30);
    }
};

TEST_F(Server, NormalConnection){
    GTEST_SKIP();
    serverRendezvous();

    EXPECT_TRUE(Bedrock::init());
    BedrockConnetion conn;
    conn.port = 8000;
    EXPECT_TRUE(Bedrock::startDedicatedHost(conn));

    // Server has started. Tell client to proceed
    serverSendSignal();

    // Wait for client to finish their tests before proceeding
    serverWaitForSignal();

    EXPECT_TRUE(Bedrock::stopDedicatedHost());
    EXPECT_TRUE(Bedrock::shutdown());

    evaluateFlags();
}

TEST_F(Server, AbruptClientDisconnect){
    GTEST_SKIP();
    serverRendezvous();

    Bedrock::init();
    BedrockConnetion conn;
    conn.port = 8000;
    Bedrock::startDedicatedHost(conn);

    // Server has started. Tell client to proceed
    serverSendSignal();

    // Wait for client kill to happen
    wait(10000);

    Bedrock::stopDedicatedHost();
    Bedrock::shutdown();
}

TEST_F(Server, SendMessageToHost){
    serverRendezvous();

    //Register a callback for receiving the greeting
    registerCallback(receiveGreeting);

    Bedrock::init();
    BedrockConnetion conn;
    conn.port = 8000;
    Bedrock::startDedicatedHost(conn);

    // Server has started. Tell client to proceed
    serverSendSignal();

    // Wait for client to finish their tests before proceeding
    serverWaitForSignal();


    // Shutdown server
    Bedrock::stopDedicatedHost();
    Bedrock::shutdown();
}