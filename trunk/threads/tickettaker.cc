// TicketTaker
#include "movietheater.h"

TicketTaker::TicketTaker(int ttId) : Employee(ttId, "TicketTaker") {
   // printf("construct ticket clerk %d\n", ttId);
}

void TicketTaker::checkTickets() {

    while(true) {
        // stop ticket check
//        lCheckTickets->Acquire();
        lTicketTaken->Acquire();
        if (stopTicketTaken == true) {
            lock->Acquire();
            stopTicketTakenArr[getId()] = true;
            lock->Release();
            DEBUGINFO('c', "%s [%d] stop taking ticket at start", getEmployeeType(), getId());
            lTicketTaken->Release();
            lCheckTickets->Acquire();
            // broadcast the waiting list
            condition[0]->Broadcast(lCheckTickets);
            DEBUGINFO('c', "%s [%d] set not busy", getEmployeeType(), getId());
//            setIsBusy(false);
            lCheckTickets->Release();
            // wait for manager to broadcast
            lTicketTaken->Acquire();
            cTicketTaken->Wait(lTicketTaken);
            DEBUGINFO('c', "%s [%d] get start taking ticket from stop", getEmployeeType(), getId());
        }
        lock->Acquire();
        stopTicketTakenArr[getId()] = false;
        lock->Release();
        lTicketTaken->Release();
        DEBUGINFO('c', "%s [%d] get in", getEmployeeType(), getId());
            
        
        DEBUGINFO('c', "%s [%d] get line lock", getEmployeeType(), getId());
        lCheckTickets->Acquire();
        if (getIsBreak()) {
            DEBUGINFO('c', "%s [%d] get on break signal", getEmployeeType(), getId());
            // wait for manager signal
            printf("%s [%d] is going on break.\n", getEmployeeType(), getId());
            lCheckTickets->Release();
            sNoTicketTaker->P();
            DEBUGINFO('c', "%s [%d] get off break", getEmployeeType(), getId());
            printf("%s [%d] is coming off break.\n", getEmployeeType(), getId());
            lCheckTickets->Acquire();
            DEBUGINFO('c', "%s [%d] get line lock", getEmployeeType(), getId());
            setIsBreak(false);

//            printf("TicketTaker [%d] is going on break.\n",getId());
//            continue;
        }

        if (getWaitingSize() > 0) {
            DEBUGINFO('c', "%s [%d] get WaitingSize: %d", getEmployeeType(), getId(), getWaitingSize());
            printf("TicketTaker [%d] has a line length [%d] and is signaling a customer.\n",getId(),getWaitingSize());
            setIsBusy(true);
            condition[0]->Signal(lCheckTickets);
        } else {
            DEBUGINFO('c', "WaitingSize:%s [%d] get line size = 0", getEmployeeType(), getId());
            printf("TicketTaker [%d] has no one in line. I am available for a customer.\n",getId());
            setIsBusy(false);
        }
        lock->Acquire();
        DEBUGINFO('c', "%s [%d] get work lock and wait for customer or manager, busy: %d", getEmployeeType(), getId(), getIsBusy()?1:0);
        // on service
        lCheckTickets->Release();
        // wait customer to signal
        condition[1]->Wait(lock);
        DEBUGINFO('c', "%s [%d] get wake up and get back work lock", getEmployeeType(), getId());
        DEBUGINFO('c', "%s [%d] acquire lCheckTickets, lCheckTickets's owner is: %s", getEmployeeType(), getId(), lCheckTickets->getOwnerThread()==NULL?"NULL":lCheckTickets->getOwnerThread()->getName());
        // if break ? race condition 
        lCheckTickets->Acquire();
        DEBUGINFO('c', "%s [%d] get line lock", getEmployeeType(), getId());
        if (getIsBreak() && !getIsBusy()) {
            DEBUGINFO('c', "%s [%d] get on break", getEmployeeType(), getId());
            setIsBusy(false);
            lock->Release();
            lCheckTickets->Release();
            continue;
        } else {
            setIsBusy(true);
        }
        lCheckTickets->Release();

        DEBUGINFO('c', "%s [%d] receives [%d] tickets from group [%d]", getEmployeeType(), getId(), getTicketSum(), getGroupId());
        //get tickets sum
        printf("TicketTaker [%d] has received [%d] tickets.\n",getId(), getTicketSum());

        DEBUGINFO('c', "%s [%d] acquire lTicketTaken, lTicketTaken's owner is %s ", getEmployeeType(), getId(), lTicketTaken->getOwnerThread() == NULL? "NULL" : lTicketTaken->getOwnerThread()->getName());
        // if stopped
        lock->Release();
        lTicketTaken->Acquire();
//        if (stopTicketTaken) {
//            lTicketTaken->Release();
//            continue;
//        }
        // if too much, has to stop
        // ? if there is another ticketTaker on process, still not stop him
        lock->Acquire();
        DEBUGINFO('c', "%s [%d] check ticket sum, ticket num: %d, all ticket: %d", getEmployeeType(), getId(), getTicketSum(), ticketTaken);
        if (stopTicketTaken || ticketTaken + getTicketSum() > 25) {
            DEBUGINFO('c', "%s [%d] get too much ticket and not to let in", getEmployeeType(), getId());
            stopTicketTaken = true;
            stopTicketTakenArr[getId()] = true;
            printf("TicketTaker [%d] is not allowing the group into the theater. The number of taken tickets is [%d] and the group size is [%d].\n", getId(), ticketTaken, getTicketSum());
            // signal the service one
            printf("TicketTaker [%d] has stopped taking tickets.\n", getId());
            condition[1]->Signal(lock);
            lock->Release();
            lTicketTaken->Release();
            // broadcast the waiting list
            lCheckTickets->Acquire();
            condition[0]->Broadcast(lCheckTickets);
            lCheckTickets->Release();
            
            continue;
        }
        ticketTaken += getTicketSum();
        totalTicketTaken += getTicketSum();
        printf("TicketTaker [%d] is allowing the group into the theater. The number of tickets taken is [%d].\n",getId(), ticketTaken);
        lTicketTaken->Release();
        // should just let in, not tell customers seats number 
        // better not to enforce seats number, which needs more variables to state
        condition[1]->Signal(lock);
        DEBUGINFO('c', "%s [%d] allows customer group [%d] get in", getEmployeeType(), getId(), getGroupId());
        // customer leave, get next customer
        condition[1]->Wait(lock);
        // put lTicketTaken Release here to avoid race condition;
        
        DEBUGINFO('c', "%s [%d] knows customer group [%d] leave", getEmployeeType(), getId(), getGroupId());
        lock->Release();
    }
}

void TicketTaker::setTicketSum(int sum) {
    ticketSum = sum; 
}

int TicketTaker::getTicketSum() {
    return ticketSum;
}

