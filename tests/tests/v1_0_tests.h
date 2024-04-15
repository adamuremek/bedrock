#ifndef TESTS_V1_0_TESTS_H
#define TESTS_V1_0_TESTS_H

class TestFixture : public ::testing::Test{
protected:
    void TearDown() override {
        IPC::endTests();
        VirtualNetwork::resetNetwork();
    }
};

TEST_F(TestFixture, Test1){
    topology1();
    IPC::startTests(2);

    IPC::spawnProcess("Host1", "clientBehavior1");
    IPC::spawnProcess("Host2", "clientBehavior2");

    IPC::waitForAllProcesses();
    IPC::evaluateTestResults();
}

#endif //TESTS_V1_0_TESTS_H
