// Manager
#include "movietheater.h"

void Manager::work() {
    while (true) {
        randToBreak(lBuyTickets, (Employee**)tc, MAX_TC);
        randToBreak(lBuyFood, (Employee**)cc, MAX_CC);
        randToBreak(lCheckTickets, (Employee**)tt, MAX_TT);
        startTicketTaken();
        startMovie();
        for (int i = 0;i < 200; ++i) {
            currentThread->Yield();
        }

    }
}
// ask MT to startMovie
void Manager::startMovie() {
    lTicketTaken->Acquire();
    int i;
    for (i = 0; i < MAX_TT; ++i) {
        if (tt[i]->getIsBusy()) {
            break;
        }
    }
    if (i == MAX_TT || stopTicketTaken) {  //stopTicketTaken ) {
        stopTicketTaken = true;
        sStartMovie->V();
    }
    lTicketTaken->Release();
}
// ask ticketTaker and customer to start
void Manager::startTicketTaken() {
    lIsMovieOver->Acquire();
    if (bIsMovieOver == true) {
        bIsMovieOver = false;
        lTicketTaken->Acquire();
        // set false
        stopTicketTaken = false;
        // ask customer and ticket clert to start ticket taken
        cTicketTaken->Broadcast(lTicketTaken);
        lTicketTaken->Release();
    }
    lIsMovieOver->Release();
}
// rand ask employee to break
void Manager::randToBreak(Lock * lockWaiting, Employee ** clerk, int count) {
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
                clerk[i]->condition[0]->Broadcast(clerk[i]->lock);
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
                clerk[i]->condition[0]->Broadcast(clerk[i]->lock);

                // if not busy, manager should signal clerk to get break
                if (!clerk[i]->getIsBusy()) {
                    clerk[i]->condition[1]->Signal(clerk[i]->lock);
                }
                printf("Manager has told %s [%d] to go on break.\n", clerk[i]->getEmployeeType(), i);
                clerk[i]->lock->Release();
            }
        } else if (clerk[i]->getWaitingSize() > 5) {
            int j = 0;
            for (j = 0;j < count; ++j) {
                if (i == j) continue;
                if (clerk[j]->getIsBreak()) {
                    clerk[j]->lock->Acquire();
                    clerk[j]->setIsBreak(false);
                    clerk[j]->lock->Release();
                    break;
                }
            }
        }
    }
    lockWaiting->Release();

}
Manager::Manager(int mrId) : Employee(mrId, "Manager"){
}

