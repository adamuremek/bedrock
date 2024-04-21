#ifndef TESTS_CLIENT_SERVER_MESSAGING_H
#define TESTS_CLIENT_SERVER_MESSAGING_H

class ClientServerMessaging: public ::testing::Test{
protected:
    void TearDown() override {
        IPC::endTests();
        VirtualNetwork::resetNetwork();
    }
};

TEST_F(ClientServerMessaging, t1){
    topology2();

    IPC::spawnProcess("Server", "serverBehavior2");
    IPC::waitForSignal("serverStarted");
    pid_t pid = IPC::spawnProcess("Client", "clientSendNumberMessage");

    IPC::waitForProcess(pid);

    IPC::postSignal("stopServer");

    IPC::waitForAllProcesses();
}

#endif //TESTS_CLIENT_SERVER_MESSAGING_H
