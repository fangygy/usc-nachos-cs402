// TicketTaker
#include "movietheater.h"

TicketTaker::TicketTaker(int ttId) : Employee(ttId, "TicketTaker") {
   // printf("construct ticket clerk %d\n", ttId);
}

void TicketTaker::checkTickets() {
    while(true) {
        if (getIsBreak()) {
            printf("TicketTaker [%d] is going on break.\n",getId());
            continue;
        }

        lCheckTickets->Acquire();
        if (getWaitingSize() > 0) {
            printf("TicketTaker [%d] has a line length [%d] and is signaling a customer.\n",getId(),getWaitingSize());
            setIsBusy(true);
            subWaitingSize();
            condition[0]->Signal(lCheckTickets);
        } else {
            printf("TicketTaker [%d] has no one in line. I am available for a customer.\n",getId());
            setIsBusy(false);
        }
        // on service
        lock->Acquire();
        lCheckTickets->Release();
        // wait customer to signal
        condition[1]->Wait(lock);
        //get tickets sum
        printf("TicketTaker [%d] has received [%d] tickets.\n",getId(),getTicketSum());
        // TODO: ? should just let in or tell customers seats number 
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

