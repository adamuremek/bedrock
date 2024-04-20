#ifndef TESTS_BEHAVIORS_H
#define TESTS_BEHAVIORS_H


void generalBehavior1(){
    bool res = Bedrock::init();
    IPC::addTestResult("Initialize bedrock", res);
}

void generalBehavior2(){
    bool res = Bedrock::init();
    IPC::addTestResult("Should have initialized", res);
    IPC::addTestResult("Double check initialization", Bedrock::isInitialized);

    Bedrock::onClientConnect += [](Bedrock::ClientID id) -> void{};
    Bedrock::onClientDisconnect += [](Bedrock::ClientID id) -> void{};

    int callbackCount = Bedrock::onClientConnect.count() + Bedrock::onClientDisconnect.count();

    IPC::addTestResult("There should be a callback for each connection event", (callbackCount == 2));

    Bedrock::shutdown();

    IPC::addTestResult("Should now be shut down", !Bedrock::isInitialized);

    callbackCount = Bedrock::onClientConnect.count() + Bedrock::onClientDisconnect.count();

    IPC::addTestResult("There should now be no callbacks for connection events", (callbackCount == 0));
}

void serverPassiveListen(){
    Bedrock::init();

    Bedrock::onClientConnect += [](Bedrock::ClientID id) -> void{
        cout << id << " CONNECTED [TEST]" << endl;
    };

    Bedrock::onClientDisconnect += [](Bedrock::ClientID id) -> void{
        cout << id << " DISCONNECTED [TEST]" << endl;
    };

    Bedrock::startDedicatedHost(8000);
    IPC::postSignal("serverStarted");

    IPC::waitForSignal("stopServer");

    Bedrock::shutdown();
}

void serverBehavior1(){
    Bedrock::init();

    Bedrock::onClientConnect += [](Bedrock::ClientID id) -> void{
      cout << "CLIENT CONNECTED [TEST]" << endl;
    };

    Bedrock::onClientDisconnect += [](Bedrock::ClientID id) -> void{
      cout << "CLIENT DISCONNECTED [TEST]" << endl;
      IPC::postSignal("stopServer");
    };

    Bedrock::startDedicatedHost(8000);
    IPC::postSignal("serverStarted");

    IPC::waitForSignal("stopServer");

    Bedrock::shutdown();
}


/**
 * Topology Used: 2
 * Number of Tests: 0
 */
void clientConnectNormalDisconnect(){
    // Uses topology 2
    Bedrock::init();

    Bedrock::onHostConnect += []() -> void{
        cout << "CONNECTED TO HOST" << endl;
    };

    Bedrock::onHostDisconnect += []() -> void{
        cout << "DISCONNECTED FROM HOST" << endl;
    };

    Bedrock::startClient(8000, "10.0.0.1");

    IPC::sleep(5);

    Bedrock::shutdown();
}

/**
 * Topology Used: 2
 * Number of Tests: 0
 */
void clientConnectAbruptDisconnect(){
    // Uses topology 2
    Bedrock::init();

    Bedrock::startClient(8000, "10.0.0.1");

    IPC::sleep(1000);

    exit(EXIT_SUCCESS);
}


void registerBehaviors(){
    IPC::registerBehavior("generalBehavior1", generalBehavior1);
    IPC::registerBehavior("generalBehavior2", generalBehavior2);
    IPC::registerBehavior("serverBehavior1", serverBehavior1);
    IPC::registerBehavior("serverPassiveListen", serverPassiveListen);
    IPC::registerBehavior("clientConnectNormalDisconnect", clientConnectNormalDisconnect);
    IPC::registerBehavior("clientConnectAbruptDisconnect", clientConnectAbruptDisconnect);
}


#endif //TESTS_BEHAVIORS_H
