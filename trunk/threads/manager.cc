// Manager
#include "movietheater.h"

void Manager::work() {
    while (true) {
        randToBreak(lBuyTickets, (Employee**)tc, tcNumber, noTicketClerk, cNoTicketClerk, sNoTicketClerk);
//        randToBreak(lBuyFood, (Employee**)cc, ccNumber, noConcessionClerk, cNoConcessionClerk, sNoConcessionClerk);
        DEBUGINFO('c', "%s check noTicketTaker: %d", getEmployeeType(), noTicketTaker?1:0 );
//        randToBreak(lCheckTickets, (Employee**)tt, ttNumber, noTicketTaker, cNoTicketTaker, sNoTicketTaker);
        startTicketTaken();
        startMovie();
        for (int i = 0;i < 10; ++i) {
            currentThread->Yield();
        }
        // if all customer leave, collect money
        lCustomerLeft->Acquire();
    DEBUG('z',"\tlCustomerLeft: %d \n", customerLeft);
        if (customerLeft == 0) {
            collectMoney();
            lCustomerLeft->Release();
            break;
        }
        lCustomerLeft->Release();
    }
}
void Manager::collectMoney() {
    lAmount->Acquire();
    int i;
    for (i = 0;i < tcNumber; ++i) {
        totalAmount += ticketClerkAmount[i];
        printf("Manager collected [%.2f] from TicketClerk[%d].\n", ticketClerkAmount[i], i);
        ticketClerkAmount[i] = 0;
    }

    for (i = 0;i < ccNumber; ++i) {
        totalAmount += concessionClerkAmount[i];
        printf("Manager collected [%.2f] from ConcessionClerk[%d].\n", concessionClerkAmount[i], i);
        concessionClerkAmount[i] = 0;
    }
    printf("Total money made by office = [%.2f]\n", totalAmount);
    lAmount->Release();
    lTicketTaken->Acquire();
    printf("Total tickets sold by office = [%d]\n", totalTicketSold);
    lTicketTaken->Release();
    lTicketSold->Acquire();
    printf("Total tickets taken by office = [%d]\n", totalTicketTaken);
    lTicketSold->Release();
}
// ask MT to startMovie
void Manager::startMovie() {
    lMovieState->Acquire();
    if (movieState == 1) {
        DEBUGINFO('c', "%s acquire lTicketTaken, lTicketTaken's owner : %s", getEmployeeType(), lTicketTaken->getOwnerThread() == NULL? "NULL": lTicketTaken->getOwnerThread()->getName());
        lTicketTaken->Acquire();
        DEBUGINFO('c', "%s acquire lTicketSold, lTicketSold's owner : %s", getEmployeeType(), lTicketSold->getOwnerThread() == NULL? "NULL": lTicketSold->getOwnerThread()->getName());
        lTicketSold->Acquire();
        DEBUG('z',"\ttotalTicketTaken: %d, totalTicketSold: %d\n", totalTicketTaken, totalTicketSold);
        DEBUGINFO('c', "%s totalTicketTaken: %d, totalTicketSold: %d, ticketTaken: %d", getEmployeeType(), totalTicketTaken, totalTicketSold, ticketTaken);
        if (stopTicketTaken || (totalTicketTaken == totalTicketSold && ticketTaken != 0)) {
            DEBUGINFO('c', "%s totalTicketTaken: %d, totalTicketSold: %d, ticketTaken: %d", getEmployeeType(), totalTicketTaken, totalTicketSold, ticketTaken);
            stopTicketTaken = true;
            movieState = 2;
            sStartMovie->V();
        }
        lTicketTaken->Release();
        lTicketSold->Release();
    }
    lMovieState->Release();
    
}
// ask ticketTaker and customer to start
void Manager::startTicketTaken() {
    DEBUGINFO('c', "%s acquire lMovieState, lMovieState's owner : %s", getEmployeeType(), lMovieState->getOwnerThread() == NULL? "NULL": lMovieState->getOwnerThread()->getName());
    lMovieState->Acquire();
    DEBUGINFO('c', "%s finish acquire lMovieState, movieState: %d", getEmployeeType(), movieState);
    if (movieState == 0) {
        DEBUGINFO('c', "%s acquire lTicketTaken, lTicketTaken's owner : %s", getEmployeeType(), lTicketTaken->getOwnerThread() == NULL? "NULL": lTicketTaken->getOwnerThread()->getName());
        lTicketTaken->Acquire();
        DEBUGINFO('c', "%s finish acquire lTicketTaken, stopTicketTaken: %d", getEmployeeType(), stopTicketTaken?1:0);
        if (stopTicketTaken) {
            
            DEBUGINFO('c', "%s acquire lTicketTaken, lTicketTaken's owner : %s", getEmployeeType(), lTicketTaken->getOwnerThread() == NULL? "NULL": lTicketTaken->getOwnerThread()->getName());
            stopTicketTaken = false;
            // set state to 1
            movieState = 1;
            ticketTaken = 0;
            DEBUGINFO('c', "%s startTicketTaken", getEmployeeType() );
            // ask customer and ticket clert to start ticket taken
            cTicketTaken->Broadcast(lTicketTaken);
        }
        lTicketTaken->Release();
    }
    lMovieState->Release();
}
// rand ask employee to break
void Manager::randToBreak(Lock * lockWaiting, Employee ** clerk, int count, bool noClerk, Condition *cNoClerk, Semaphore * sNoClerk) {
    // lock to get in line
    lockWaiting->Acquire();
    for (int i = 0;i < count; ++i) {
        if (clerk[i]->getIsBreak()) continue;
        if (!clerk[i]->getWaitingSize()) {
            // rand to break
            if (rand()%5 == 0) {
                clerk[i]->lock->Acquire();
                clerk[i]->setIsBreak(true);
                // signal the waiting customer
                clerk[i]->condition[0]->Broadcast(lockWaiting);
                if (!clerk[i]->getIsBusy()) {
                    clerk[i]->condition[1]->Signal(clerk[i]->lock);
                }
                printf("Manager has told %s [%d] to go on break.\n", clerk[i]->getEmployeeType(), i);
                clerk[i]->lock->Release();
            }
        } else if (clerk[i]->getWaitingSize() < 3) {
            int j = 0;
            for (j = 0;j < count; ++i) {
                if (i == j) continue;
                if (!clerk[j]->getIsBreak()) {
                    break;
                }
            }
            if (j != count && rand()%5 == 0) {
                clerk[i]->lock->Acquire();
                clerk[i]->setIsBreak(true);
                // signal the waiting customer
                clerk[i]->condition[0]->Broadcast(lockWaiting);

                // if not busy, manager should signal clerk to get break
                if (!clerk[i]->getIsBusy()) {
                    clerk[i]->condition[1]->Signal(clerk[i]->lock);
                }
                printf("Manager has told %s [%d] to go on break.\n", clerk[i]->getEmployeeType(), i);
                clerk[i]->lock->Release();
            }
        } else if (clerk[i]->getWaitingSize() > 5) {
            /*int j = 0;
            for (j = 0;j < count; ++j) {
                if (i == j) continue;
                if (clerk[j]->getIsBreak()) {
                    clerk[j]->lock->Acquire();
                    clerk[j]->setIsBreak(false);
                    clerk[j]->lock->Release();
                    break;
                }
            }*/
            sNoClerk->V();
        }
    }
    // if no clerk work and customers come, have to ask one to off break
    if (noClerk) {
        DEBUGINFO('c', "noClerk %s, should wake up someone", clerk[0]->getEmployeeType());
        noClerk = false;
        // semaphore to get one clerk
        DEBUGINFO('c', "sNoClerk %s", clerk[0]->getEmployeeType());
        sNoClerk->V();
        // ? should get resp from clerk
        // signal the customer
        cNoClerk->Broadcast(lockWaiting);
    }
    lockWaiting->Release();

}
Manager::Manager(int mrId) : Employee(mrId, "Manager"){
}

