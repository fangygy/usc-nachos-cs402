// TicketTaker
#include "movietheater.h"

TicketTaker::TicketTaker(int ttId) : Employee(ttId, "TicketTaker") {
   // printf("construct ticket clerk %d\n", ttId);
}

void TicketTaker::checkTickets() {

    while(true) {
        // stop ticket check
        lTicketTaken->Acquire();
        if (stopTicketTaken == true) {
            // wait for manager to broadcast
            cTicketTaken->Wait(lTicketTaken);
        }
        lTicketTaken->Release();
            
        lCheckTickets->Acquire();
        if (getIsBreak()) {
            // wait for manager signal
            printf("%s [%d] is going on break.\n", getEmployeeType(), getId());
            lCheckTickets->Release();
            sNoTicketTaker->P();
            printf("%s [%d] is coming off break.\n", getEmployeeType(), getId());
            lCheckTickets->Acquire();
            setIsBreak(false);

//            printf("TicketTaker [%d] is going on break.\n",getId());
//            continue;
        }

        if (getWaitingSize() > 0) {
            printf("TicketTaker [%d] has a line length [%d] and is signaling a customer.\n",getId(),getWaitingSize());
            setIsBusy(true);
            subWaitingSize();
            condition[0]->Signal(lCheckTickets);
        } else {
            printf("TicketTaker [%d] has no one in line. I am available for a customer.\n",getId());
            setIsBusy(false);
        }
        lock->Acquire();
        // on service
        lCheckTickets->Release();
        // wait customer to signal
        condition[1]->Wait(lock);
        lock->Release();
        // if break ? race condition 
        lCheckTickets->Acquire();
        if (getIsBreak()) {
            setIsBusy(false);
            lCheckTickets->Release();
            continue;
        } else {
            setIsBusy(true);
        }
        lCheckTickets->Release();

        lock->Acquire();
        //get tickets sum
        printf("TicketTaker [%d] has received [%d] tickets.\n",getId(), getTicketSum());

        // if stopped
        lTicketTaken->Acquire();
//        if (stopTicketTaken) {
//            lTicketTaken->Release();
//            continue;
//        }
        // if too much, has to stop
        // ? if there is another ticketTaker on process, still not stop him
        if (ticketTaken + getTicketSum() > 25) {
            stopTicketTaken = true;
            printf("TicketTaker [%d] is not allowing the group into the theater. The number of taken tickets is [%d] and the group size is [%d].\n", getId(), ticketTaken, getTicketSum());
            // signal the service one
            printf("TicketTaker [%d] has stopped taking tickets.\n", getId());
            condition[1]->Signal(lock);
            lock->Release();
            // broadcast the waiting list
            lCheckTickets->Acquire();
            condition[0]->Broadcast(lCheckTickets);
            lCheckTickets->Release();
            lTicketTaken->Release();
            continue;
        }
        ticketTaken += getTicketSum();
        totalTicketTaken += getTicketSum();
        printf("TicketTaker [%d] is allowing the group into the theater. The number of tickets taken is [%d].\n",getId(), ticketTaken);
        lTicketTaken->Release();
        // should just let in, not tell customers seats number 
        // better not to enforce seats number, which needs more variables to state
        condition[1]->Signal(lock);
        // customer leave, get next customer
        condition[1]->Wait(lock);
        lock->Release();
    }
}

void TicketTaker::setTicketSum(int sum) {
    ticketSum = sum; 
}

int TicketTaker::getTicketSum() {
    return ticketSum;
}

