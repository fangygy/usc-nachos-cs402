// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    DEBUGINFO('c', "%s P() %s value %d", currentThread->getName(), getName(), value);    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    DEBUGINFO('c', "%s V() %s value %d", currentThread->getName(), getName(), value);
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) {
    #ifdef CHANGED
    name = debugName;
    isUsed = false;
    waitingQueue = new List;
    #endif
}
Lock::~Lock() {
    #ifdef CHANGED
    delete waitingQueue;
    #endif
}
void Lock::Acquire() {
    #ifdef CHANGED
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    if (isHeldByCurrentThread()) {    // check if currentThread is the owner of lock
        (void) interrupt->SetLevel(oldLevel);
        return;
    }

    if (isUsed) {   // if the lock is used
        waitingQueue->Append((void *)currentThread);   // append to wait and go to sleep
        currentThread->Sleep(); 
    } else {
        isUsed = true;
        ownerThread = currentThread;
    } 
    DEBUGINFO('c', "Lock [%s] acquired by [%s]", getName(), getOwnerThread()->getName());
    (void) interrupt->SetLevel(oldLevel);
    #endif
}
void Lock::Release() {
    #ifdef CHANGED
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    if (!isHeldByCurrentThread()) {    // check if currentThread is the owner of lock
        printf("You are not the owner. Only the lock owner can release it\n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }

    DEBUGINFO('c', "Lock [%s] released by [%s]", getName(), getOwnerThread()->getName());
    Thread *thread = (Thread *)waitingQueue->Remove();
    if (thread != NULL) {    // check if threads wait for acquiring lock
        DEBUGINFO('c', "Lock [%s] give to [%s] by [%s]", getName(), thread->getName(), getOwnerThread()->getName());
        ownerThread = thread;
        scheduler->ReadyToRun(thread);   // append to ready queue
    } else {                 // release lock
        ownerThread = NULL;
        isUsed = false;
    }

    (void) interrupt->SetLevel(oldLevel);
    #endif
}

Thread* Lock::getOwnerThread() {
    return ownerThread;
}

#ifdef CHANGED
bool Lock::isHeldByCurrentThread() {
    return (ownerThread != NULL && ownerThread == currentThread);
}
#endif
Condition::Condition(char* debugName) {
    #ifdef CHANGED
    name = debugName;
    waitingQueue = new List;
    #endif
}
Condition::~Condition() {
    delete waitingQueue;
}
void Condition::Wait(Lock* conditionLock) { 
    #ifdef CHANGED
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    // check if the conditionLock is NULL and is held by currentThread
    if (conditionLock == NULL || !conditionLock->isHeldByCurrentThread()) {
        DEBUGINFO('c', "ERROR: You do not have a lock\n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
    // if waitingLock is NULL, put conditionLock to it
    if (waitingLock == NULL) {
        waitingLock = conditionLock;
    }
    // if waitingLock is not conditionLock, should return for wrong lock
    if (waitingLock != conditionLock) {
        DEBUGINFO('c', "ERROR: Wrong lock for waiting.\n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
    // add myself to condition waiting queue
    waitingQueue->Append((void *) currentThread); 
    // give up conditionLock
    conditionLock->Release();
    // wait for schedule 
    currentThread->Sleep();
    DEBUGINFO('c', "%s wake up", currentThread->getName());
    // require conditionLock
    DEBUGINFO('c', "%s acquire lock %s, owner %s", currentThread->getName(), conditionLock->getName(), conditionLock->getOwnerThread()!=NULL?conditionLock->getOwnerThread()->getName():"NULL");
    conditionLock->Acquire();

    DEBUGINFO('c', "%s acquired lock %s, owner %s", currentThread->getName(), conditionLock->getName(), conditionLock->getOwnerThread()!=NULL?conditionLock->getOwnerThread()->getName():"NULL");
    (void) interrupt->SetLevel(oldLevel);
    #endif
}
void Condition::Signal(Lock* conditionLock) {
    #ifdef CHANGED
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    
    // check if the conditionLock is NULL and is held by currentThread
    if (conditionLock == NULL || !conditionLock->isHeldByCurrentThread()) {
        DEBUGINFO('c', "ERROR: You do not have a lock to signal other\n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }

    // check conditionLock is waiting for 
    if (waitingLock == NULL) {
        //printf("No lock wait.\n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
    if (waitingLock != conditionLock) {
        DEBUGINFO('c', "ERROR: Pass the wrong lock.\n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }

    // put the first waiting thread to ready queue 
    Thread *thread = (Thread *)waitingQueue->Remove();
    if (thread == NULL) {
        DEBUGINFO('c', "%s with lock %s, No thread in waiting",currentThread->getName(), waitingLock->getName());
        waitingLock = NULL;
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
DEBUGINFO('c', "%s signal with lock %s, thread [%s]", currentThread->getName(), waitingLock->getName(), thread->getName());
    scheduler->ReadyToRun(thread);
    if (waitingQueue->IsEmpty()) {
        waitingLock = NULL; 
    }    
    (void ) interrupt->SetLevel(oldLevel);
    #endif
}
void Condition::Broadcast(Lock* conditionLock) {
    #ifdef CHANGED
    while (!waitingQueue->IsEmpty()) {    // if waiting queue not empty
        Signal(conditionLock);
    }
    #endif
}
