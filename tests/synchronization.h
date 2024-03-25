#ifndef TESTS_SYNCHRONIZATION_H
#define TESTS_SYNCHRONIZATION_H
#include <windows.h>
#include <iostream>

#ifdef SYNC_DEBUG
#define DEBUG_PRINT(TEXT) std::cout << TEXT << std::endl
#define DEBUG_ERR(TEXT) std::cerr << TEXT << std::endl
#else
#define DEBUG_PRINT(TEXT)
#define DEBUG_ERR(TEXT)
#endif


const int MAX_ATTEMPTS = 10;

void rendezvous(const char* eventIdA, const char* eventIdB){
    // Step 1: Create/Open named event for Process A
    HANDLE eventA = CreateEvent(nullptr, TRUE, FALSE, eventIdA);
    if (eventA == nullptr) {
        DEBUG_ERR("CreateEvent failed for event.");
        return;
    }

    // Step 3: Open the named event for Process B and wait for it
    HANDLE eventB = nullptr;
    int attempts = 0;

    while(attempts < MAX_ATTEMPTS){
        eventB = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventIdB);

        if(eventB != nullptr){
            break;
        }

        Sleep(1000);
        attempts++;
    }

    if (eventB == nullptr) {
        DEBUG_ERR("OpenEvent failed for event.");
        CloseHandle(eventA); // Cleanup
        return;
    }

    // Step 2: Signal that Process A has reached the rendezvous point
    SetEvent(eventA);
    DEBUG_PRINT("Process has reached the rendezvous point.");

    // Wait for Process B's rendezvous
    DEBUG_PRINT("Waiting for rendezvous...");
    WaitForSingleObject(eventB, INFINITE);

    // Step 4: Both processes have reached the rendezvous point
    DEBUG_PRINT("Both processes have reached the rendezvous point. Continuing execution.");


    // Cleanup
    // IMPORTANT: Handles at the kernel level to not immediately disappear,
    // they wait for all references to be cleared first. Thus, CloseHandle() does not destroy the event immediately.
    CloseHandle(eventA);
    CloseHandle(eventB);
}

void waitForSignal(const char* signalId){
    HANDLE signal = CreateEvent(nullptr, TRUE, FALSE, signalId);
    if (signal == nullptr) {
        DEBUG_ERR("CreateEvent failed for signal.");
        return;
    }

    DEBUG_PRINT( "Waiting for signal...");
    WaitForSingleObject(signal, INFINITE);

    DEBUG_PRINT("Signal received! Continuing execution.");
    CloseHandle(signal);
}

void sendSignal(const char* signalId){
    HANDLE signal = OpenEvent(EVENT_ALL_ACCESS, FALSE, signalId);
    if (signal == nullptr){
        DEBUG_ERR("OpenEvent failed for signal.");
        return;
    }

    SetEvent(signal);

    DEBUG_PRINT("Signal set! Continuing execution.");
    CloseHandle(signal);
}

void wait(int timeMs){
    Sleep(timeMs);
}

void clientRendezvous(){
    rendezvous("ClientRendezvous", "ServerRendezvous");
}

void serverRendezvous(){
    rendezvous("ServerRendezvous", "ClientRendezvous");
}

void serverWaitForSignal(){
    waitForSignal("ServerWait");
}

void clientWaitForSignal(){
    waitForSignal("ClientWait");
}

void serverSendSignal(){
    sendSignal("ClientWait");
}

void clientSendSignal(){
    sendSignal("ServerWait");
}


#endif //TESTS_SYNCHRONIZATION_H
