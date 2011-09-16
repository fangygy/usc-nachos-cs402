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

void TicketClerk::sellTickets() {
    while(true) {
        lBuyTickets->Acquire();
        lock->Acquire();
        while (getIsBreak()) {
            // wait for manager signal
            printf("%s [%d] is going on break.\n", getEmployeeType(), getId());
            condition[1]->Wait(lock);
            printf("%s [%d] is coming off break.\n", getEmployeeType(), getId()); 
        }

        if (getWaitingSize() > 0) {
            setIsBusy(true);
            printf("TicketClerk [%d] has a line length [%d] and is signaling a customer\n",getId(),getWaitingSize());
            subWaitingSize();
            condition[0]->Signal(lBuyTickets);
        } else {
            printf("TicketClerk [%d] has no one in line. I am available for a customer.\n",getId());
            setIsBusy(false);
        }
      
        // on service
        lBuyTickets->Release();
        // wait customer to signal
        condition[1]->Wait(lock);
        // if break 
        if (getIsBreak()) {
            setIsBusy(false);
            lock->Release();
            continue;
        } else {
            setIsBusy(true);
        }
        // get tickets sum 
        int ticketSum = getTicketSum(); 
        // tell customers amount
        setAmount(ticketSum); 
        printf("TicketClerk [%d] has an order for [%d] and the cost is [%.2f].\n",getId(),ticketSum,getAmount()); 
      
        condition[1]->Signal(lock);
        condition[1]->Wait(lock);
        // get money, handout the tickets 
        ticketReceipt[getGroupId()] = ticketSum;
        condition[1]->Signal(lock);
        // customer leave, get next customer 
        condition[1]->Wait(lock);
        lock->Release();

    }
}
void TicketClerk::printStatus(){

    //Print the number of waiting customer in each clerk's line

    printf("[TicketClerk]:TC State:");
    for (int i = 0;i < MAX_TC; ++i) {
        TicketClerk *clerk = tc[i];
        if(clerk!=NULL){ // if clerk have not been create, don't print
            printf("%d,",clerk->getIsBusy());
        }else{
            printf("NA,"); 
        }
    }
    printf("\n");


    //Print the number of waiting customer in each clerk's line

    printf("[TicketClerk]:TC WaitLine(#):");

    for (int i = 0;i < MAX_TC; ++i) {
        TicketClerk *clerk = tc[i];
        if(clerk!=NULL){ // if clerk have not been create, don't print
            printf("%d,",clerk->getWaitingSize());
        }else{
            printf("NA,"); 
        }
    }
    printf("\n");

}
