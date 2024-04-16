#ifndef TESTS_INITIALIZATION_AND_SHUTDOWN_H
#define TESTS_INITIALIZATION_AND_SHUTDOWN_H

class InitializationAndShutdown : public ::testing::Test{
protected:
    void TearDown() override {
        IPC::endTests();
        VirtualNetwork::resetNetwork();
    }
};

TEST_F(InitializationAndShutdown, Initialize){
    IPC::startTests(1);

    IPC::spawnProcess("generalBehavior1");

    IPC::waitForAllProcesses();
    IPC::evaluateTestResults();
}

TEST_F(InitializationAndShutdown, Shutdown){
    IPC::startTests(5);

    IPC::spawnProcess("generalBehavior2");

    IPC::waitForAllProcesses();
    IPC::evaluateTestResults();
}


#endif //TESTS_INITIALIZATION_AND_SHUTDOWN_H
