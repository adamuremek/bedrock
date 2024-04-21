#ifndef TESTS_CLIENT_SERVER_MESSAGING_H
#define TESTS_CLIENT_SERVER_MESSAGING_H

class ClientServerMessaging: public ::testing::Test{
protected:
    void TearDown() override {
        IPC::endTests();
        VirtualNetwork::resetNetwork();
    }
};

TEST_F(ClientServerMessaging, VollyMessageOneClient){
    topology2();

    IPC::spawnProcess("Server", "serverBehavior2");
    IPC::waitForSignal("serverStarted");
    pid_t pid = IPC::spawnProcess("Client", "clientSendNumberMessage");

    IPC::waitForProcess(pid);

    IPC::postSignal("stopServer");

    IPC::waitForAllProcesses();
}

TEST_F(ClientServerMessaging, VollyMessageWithLayer){
    topology3();

    IPC::spawnProcess("Server", "serverBehavior3");
    IPC::waitForSignal("serverStarted");

    pid_t p1 = IPC::spawnProcess("Client1", "clientBehavior4");
    pid_t p2 = IPC::spawnProcess("Client2", "clientBehavior4");
    pid_t p3 = IPC::spawnProcess("Client3", "clientBehavior4");
    pid_t p4 = IPC::spawnProcess("Client4", "clientBehavior4");
    pid_t p5 = IPC::spawnProcess("Client5", "clientBehavior4");

    IPC::sleep(4000);

    IPC::postSignal("sendMessages");

    IPC::waitForProcess(p1);
    IPC::waitForProcess(p2);
    IPC::waitForProcess(p3);
    IPC::waitForProcess(p4);
    IPC::waitForProcess(p5);

    IPC::postSignal("stopServer");

    IPC::waitForAllProcesses();

}

#endif //TESTS_CLIENT_SERVER_MESSAGING_H
