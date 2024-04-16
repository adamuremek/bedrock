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

    Bedrock::clientConnectedToHost += []() -> void{};
    Bedrock::clientDisconnectedFromHost += []() -> void{};

    int callbackCount = Bedrock::clientConnectedToHost.count() + Bedrock::clientDisconnectedFromHost.count();

    IPC::addTestResult("There should be a callback for each connection event", (callbackCount == 2));

    Bedrock::shutdown();

    IPC::addTestResult("Should now be shut down", !Bedrock::isInitialized);

    callbackCount = Bedrock::clientConnectedToHost.count() + Bedrock::clientDisconnectedFromHost.count();

    IPC::addTestResult("There should now be no callbacks for connection events", (callbackCount == 0));
}


void registerBehaviors(){
    IPC::registerBehavior("generalBehavior1", generalBehavior1);
    IPC::registerBehavior("generalBehavior2", generalBehavior2);
}


#endif //TESTS_BEHAVIORS_H
