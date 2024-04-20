#ifndef TESTS_CONNECTIVITY_H
#define TESTS_CONNECTIVITY_H

class Connectivity: public ::testing::Test{
protected:
    void TearDown() override {
        IPC::endTests();
        VirtualNetwork::resetNetwork();
    }
};

TEST_F(Connectivity, ClientNormalConnectAndDisconnect){
    topology2();

    IPC::spawnProcess("Server", "serverBehavior1");
    IPC::waitForSignal("serverStarted");

    IPC::spawnProcess("Client", "clientConnectNormalDisconnect");

    IPC::waitForAllProcesses();
}

TEST_F(Connectivity, ClientNormalConnectAbruptDisconnect){
    topology2();

    IPC::spawnProcess("Server", "serverBehavior1");
    IPC::waitForSignal("serverStarted");

    IPC::spawnProcess("Client", "clientConnectAbruptDisconnect");

    IPC::waitForAllProcesses();
}

TEST_F(Connectivity, MultipleClientsConnectToServer){
    topology3();

    IPC::spawnProcess("Server", "serverPassiveListen");
    IPC::waitForSignal("serverStarted");

    pid_t p1 = IPC::spawnProcess("Client1" , "clientConnectNormalDisconnect");
    pid_t p2 = IPC::spawnProcess("Client2" , "clientConnectNormalDisconnect");
    pid_t p3 = IPC::spawnProcess("Client3" , "clientConnectNormalDisconnect");
    pid_t p4 = IPC::spawnProcess("Client4" , "clientConnectNormalDisconnect");
    pid_t p5 = IPC::spawnProcess("Client5" , "clientConnectNormalDisconnect");

    IPC::waitForProcess(p1);
    IPC::waitForProcess(p2);
    IPC::waitForProcess(p3);
    IPC::waitForProcess(p4);
    IPC::waitForProcess(p5);

    IPC::postSignal("stopServer");

    IPC::waitForAllProcesses();
}

#endif //TESTS_CONNECTIVITY_H
