// TicketClerk
#include "movietheater.h"

TicketClerk::TicketClerk(int tcId) : Employee(tcId, "TicketClerk") {
    //printf("construct ticket clerk %d\n", tcId);
}

void TicketClerk::setAmount(int sum) {
    amount = sum*(PRICE_TICKET);
}

double TicketClerk::getAmount() {
    return amount;
}
void TicketClerk::setTicketSum(int sum) {
    ticketSum = sum;
}

int TicketClerk::getTicketSum() {
    return ticketSum;
}


void TicketClerk::setPayment(double p) {
    payment = p;
}
 //ticket clerk sell ticket and interact with headcustomer
void TicketClerk::sellTickets() {
    while(true) {
        lBuyTickets->Acquire();
        if (getIsBreak()) {
          //if clerk is on break,wait for manager signal
            printf("%s [%d] is going on break.\n", getEmployeeType(), getId());
            lBuyTickets->Release();
            sNoTicketClerk->P();
            printf("%s [%d] is coming off break.\n", getEmployeeType(), getId()); 
            lBuyTickets->Acquire();
            setIsBreak(false);
        }
        // if have customer, then set busy and signal the customer, else set free
        if (getWaitingSize() > 0) {
            setIsBusy(true);
            printf("TicketClerk [%d] has a line length [%d] and is signaling a customer\n",getId(),getWaitingSize());
            //subWaitingSize();
            condition[0]->Signal(lBuyTickets);
        } else {
            printf("TicketClerk [%d] has no one in line. I am available for a customer.\n",getId());
            setIsBusy(false);
        }
      
        //sell ticket:interact with customer
        lock->Acquire();
        // on service
        lBuyTickets->Release();
        // wait customer to signal
        condition[1]->Wait(lock);
        // if break 
        DEBUGINFO('c', "1 TicketClerk [%d] has a line size [%d] and is signaled by a customer\n",getId(),getWaitingSize());
        DEBUGINFO('c', "TicketClerk [%d] has a line size [%d] and wait for lBuyTickets Lock owner is %s\n",getId(),getWaitingSize(), lBuyTickets->getOwnerThread()!=NULL?lBuyTickets->getOwnerThread()->getName():"NULL");
        lBuyTickets->Acquire();
        DEBUGINFO('c', "TicketClerk [%d] has a line size [%d] and is signaling a customer\n",getId(),getWaitingSize());
        if (getIsBreak() && !getIsBusy()) {
            setIsBusy(false);
            lock->Release();
            lBuyTickets->Release();
            continue;
        } else {
            setIsBusy(true);
        }
        lBuyTickets->Release();

        // get tickets sum 
        int ticketSum = getTicketSum(); 
        // tell customers amount
        setAmount(ticketSum); 
        printf("TicketClerk [%d] has an order for [%d] and the cost is [%.2f].\n",getId(),ticketSum,getAmount()); 
      
        condition[1]->Signal(lock);
        condition[1]->Wait(lock);
        DEBUGINFO('c', "2 TicketClerk [%d] has an order for [%d] and the cost is [%.2f].\n",getId(),ticketSum,getAmount()); 
        // cal amount
        lAmount->Acquire();
        ticketClerkAmount[getId()] += getAmount();
        lAmount->Release();
        // get money, handout the tickets 
        ticketReceipt[getGroupId()] = ticketSum;
        lTicketSold->Acquire();
        totalTicketSold += ticketSum;
        lTicketSold->Release();
        condition[1]->Signal(lock);
        // customer leave, get next customer 
        condition[1]->Wait(lock);
        DEBUGINFO('c',"3 TicketClerk [%d] got the lock back after customer leave\n", getId());
        lock->Release();

    }
}

