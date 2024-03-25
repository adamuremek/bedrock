#include <gtest/gtest.h>
#include <bedrock/bedrock.h>
#include "synchronization.h"
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
    void evaluateFlags(){
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
};

TEST_F(Server, NormalConnection){
    GTEST_SKIP();
    serverRendezvous();

    EXPECT_TRUE(Bedrock::init());
    EXPECT_TRUE(Bedrock::startDedicatedHost());

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
    Bedrock::startDedicatedHost();

    // Server has started. Tell client to proceed
    serverSendSignal();

    // Wait for client kill to happen
    wait(10000);

    Bedrock::stopDedicatedHost();
    Bedrock::shutdown();
}

TEST_F(Server, SendMessageToHost){
    serverRendezvous();

    Bedrock::init();
    Bedrock::startDedicatedHost();

    // Server has started. Tell client to proceed
    serverSendSignal();

    // Wait for client to finish their tests before proceeding
    serverWaitForSignal();


    // Shutdown server
    Bedrock::stopDedicatedHost();
    Bedrock::shutdown();
}