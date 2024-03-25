#ifndef TESTS_SYNCHRONIZATION_H
#define TESTS_SYNCHRONIZATION_H

#define POSIX
#define SYNC_DEBUG

#ifdef WIN_API
#include <windows.h>
#elif defined(POSIX)
#include <fcntl.h>           // For O_* constants
#include <sys/stat.h>        // For mode constants
#include <semaphore.h>
#include <cstdio>
#include <unistd.h>
#endif



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
#ifdef WIN_API
    // Step 1: Create/Open named event for Process A
    HANDLE eventA = CreateEvent(nullptr, TRUE, FALSE, eventIdA);
    if (eventA == nullptr) {
        DEBUG_ERR("CreateEvent failed for event.");
        return;
    }

    // Step 2: Open the named event for Process B and wait for it
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

    // Step 3: Signal that Process A has reached the rendezvous point
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

#elif defined(POSIX)

    // Step 1: Create/Open named semaphore for Process A
    sem_t *semA = sem_open(eventIdA, O_CREAT, 0644, 0);
    if (semA == SEM_FAILED) {
        DEBUG_ERR("sem_open failed for eventA");
        return;
    }

    // Step 2: Open the named semaphore for Process B and wait for it
    sem_t *semB = SEM_FAILED;
    int attempts = 0;

    while(attempts < MAX_ATTEMPTS){
        semB = sem_open(eventIdB, 0);
        if(semB != SEM_FAILED){
            break;
        }

        sleep(1); // Sleep for 1 second
        attempts++;
    }

    if (semB == SEM_FAILED) {
        DEBUG_ERR("sem_open failed for event.");
        sem_close(semA); // Cleanup
        sem_unlink(eventIdA); // Optionally unlink semaphore
        return;
    }

    // Step 3: Signal that Process A has reached the rendezvous point
    sem_post(semA);
    DEBUG_PRINT("Process has reached the rendezvous point.");

    // Wait for Process B's rendezvous
    DEBUG_PRINT("Waiting for rendezvous...");
    sem_wait(semB);

    // Step 4: Both processes have reached the rendezvous point
    DEBUG_PRINT("Both processes have reached the rendezvous point. Continuing execution.");

    // Cleanup
    sem_close(semA);
    sem_close(semB);
    // Optionally unlink semaphores if they are no longer needed
    sem_unlink(eventIdA);

#endif
}

void waitForSignal(const char* signalId){
#ifdef WIN_API
    HANDLE signal = CreateEvent(nullptr, TRUE, FALSE, signalId);
    if (signal == nullptr) {
        DEBUG_ERR("CreateEvent failed for signal.");
        return;
    }

    DEBUG_PRINT( "Waiting for signal...");
    WaitForSingleObject(signal, INFINITE);

    DEBUG_PRINT("Signal received! Continuing execution.");
    CloseHandle(signal);

#elif defined(POSIX)
    // Opening an existing named semaphore
    sem_t* signal = sem_open(signalId, 0);
    if (signal == SEM_FAILED) {
        DEBUG_ERR("sem_open failed for signal AHHH");
        return;
    }

    DEBUG_PRINT("Waiting for signal...");
    // Waiting (decrementing) semaphore
    if (sem_wait(signal) == -1) {
        DEBUG_ERR("sem_wait failed");
    } else {
        DEBUG_PRINT("Signal received! Continuing execution.");
    }

    // Cleanup
    sem_close(signal);
    sem_unlink(signalId);
#endif
}

void sendSignal(const char* signalId){
#ifdef WIN_API
    HANDLE signal = OpenEvent(EVENT_ALL_ACCESS, FALSE, signalId);
    if (signal == nullptr){
        DEBUG_ERR("OpenEvent failed for signal.");
        return;
    }

    SetEvent(signal);

    DEBUG_PRINT("Signal set! Continuing execution.");
    CloseHandle(signal);

#elif defined(POSIX)
    // Opening an existing named semaphore
    sem_t* signal = sem_open(signalId, O_CREAT, 0644, 1);
    if (signal == SEM_FAILED) {
        DEBUG_ERR("sem_open failed for signal 8=====D~~");
        return;
    }

    // Incrementing (signaling) semaphore
    if (sem_post(signal) == -1) {
        DEBUG_ERR("sem_post failed");
    } else {
        DEBUG_PRINT("Signal set! Continuing execution.");
    }

    // Cleanup
    sem_close(signal);
#endif
}

void wait(int timeMs){
#ifdef WIN_API
    Sleep(timeMs);
#elif defined(POSIX)
    struct timespec ts;
    ts.tv_sec = timeMs / 1000;                  // Convert milliseconds to seconds
    ts.tv_nsec = (timeMs % 1000) * 1000000L;    // Convert remainder to nanoseconds

    nanosleep(&ts, nullptr);

#endif
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
