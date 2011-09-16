// Customer
#include "movietheater.h"

Customer::Customer(int cId, int gId, int gSize, bool ticketBuyer) {
    customerId = cId;
    groupId = gId;
    groupSize = gSize;
    isTicketBuyer = ticketBuyer;
    // how to find groupmate
    //Customer * customers;   //Only head customers need this value
    printf("Customer [%d] in Group [%d] has entered the movie theater\n",customerId,groupId);
}
Customer::~Customer() {}

void Customer::waitGroup() {
    for (int i = 1;i < groupSize; ++i) {
        sGroup[groupId]->P();
    }
}
void Customer::action() {
    if (getIsTicketBuyer()) {
        // ticket buyer
        buyTickets();
        waitGroup();
        if (countFood()) {
            buyFood();
        }
        waitGroup();
        checkTickets();
        waitGroup();
        arrangeSeats();
    } else {
        // regular customer
        waitTickets();
        waitFood();
        waitSeats();
    }
}
bool Customer::getIsTicketBuyer() {
    return isTicketBuyer;
}

int Customer::getInLine(Lock *lock, Employee** employee, int count) {

    int i, j;
    int lineIndex = -1;
    int lineLen = -1;
    lock->Acquire();

    for (i = 0;i < count; ++i) {

        // get a no busy line
        if (!employee[i]->getIsBusy() && !employee[i]->getIsBreak() && employee[i]->getWaitingSize() == 0) {
            lineIndex = i;
            break;
        }
        // get a shortest line
        if (!employee[i]->getIsBreak() && (lineIndex == -1 || employee[i]->getWaitingSize() < lineLen)) {
            lineLen = employee[i]->getWaitingSize();
            lineIndex = i;
        }
    }
    
    // if no service
    if (lineIndex == -1) {
       // TODO need to wait until the manager signal them
       // Wait();
    } else {
        if(employee[lineIndex]->getIsBusy()){        
            employee[lineIndex]->addWaitingSize();
        }
    }
    return lineIndex; 
}
void Customer::buyTickets() {

    int lineIndex = -1;
    //printf("Customer %d try to get in line\n",customerId);
    //printTCStatus();

    while (lineIndex == -1) {
        lineIndex = getInLine(lBuyTickets, (Employee**)tc, MAX_TC);        
    }
    // get in which line
    printf("Customer [%d] in Group [%d] is getting in TicketClerk line [%d]\n",customerId,groupId,lineIndex);
    TicketClerk *clerk = tc[lineIndex];
    // if busy wait for ticketClerk
    if (clerk->getIsBusy()) {
        clerk->condition[0]->Wait(lBuyTickets);
    }
    printf("Customer [%d] in Group [%d] is walking up to TicketClerk[%d] to buy [%d] tickets\n",customerId,groupId,lineIndex,groupSize);
    //printf("customer %d : is interacting with clerk %d\n", customerId, lineIndex);
    //printTCStatus();
    //printf("============================\n");
    // interact with TicketClerk
    clerk->lock->Acquire();
    if (!clerk->getIsBreak()) {
        lBuyTickets->Release(); 
        // tell clerk tickets sum
        clerk->setTicketSum(groupSize);
        clerk->condition[1]->Signal(clerk->lock);

        // wait for money amount
        clerk->condition[1]->Wait(clerk->lock);
        // ask ticket clerk for total price
        double amount = clerk->getAmount();
        // pay money 
        clerk->setPayment(amount);
        printf("Customer [%d] in Group [%d] in TicketClerk line [%d] is paying [%.2f] for tickets\n",customerId,groupId,lineIndex,amount);
        
        clerk->condition[1]->Signal(clerk->lock);

        // wait to get tickets back
        clerk->condition[1]->Wait(clerk->lock);
        // finish and leave
        clerk->condition[1]->Signal(clerk->lock);
        printf("Customer [%d] in Group [%d] is leaving TicketClerk[%d]\n",customerId,groupId,lineIndex);
    } else {
        printf("Customer [%d] in Group [%d] sees TicketClerk [%d] is on break.\n",customerId,groupId,lineIndex);
        clerk->subWaitingSize();
        // for race condition of waiting size
        lBuyTickets->Release();
        clerk->lock->Release();
        buyTickets();
        return;
    }
    clerk->lock->Release();
    // tell group to go
    lGroup[groupId]->Acquire();
    cGroup[groupId]->Broadcast(lGroup[groupId]);
    // monitor variables
    groupTicket[groupId] = true;
    lGroup[groupId]->Release();
}

void Customer::buyFood() {
    int lineIndex = -1;
    while (lineIndex == -1) {
        lineIndex = getInLine(lBuyFood, (Employee**)cc, MAX_CC);
    }
    printf("Customer [%d] in Group [%d] is getting in ConcessionClerk line [%d]\n",customerId,groupId,lineIndex);

    ConcessionClerk *clerk = cc[lineIndex];
    if (clerk->getIsBusy()) {
        clerk->condition[0]->Wait(lBuyFood);
    }
    printf("Customer [%d] in Group [%d] is walking up to ConcessionClerk[%d] to buy [%d] popcorn and [%d] soda.\n",customerId,groupId,lineIndex,groupFoodSum[groupId][0],groupFoodSum[groupId][1]);
    // interact with ConcessionClerk 
    clerk->lock->Acquire();
    if (!clerk->getIsBreak()) {
        lBuyFood->Release();
        // TODO: tell clerk popcorn sum and soda sum

        for (int i = 0;i < 2; ++i) {
            clerk->setFood(i, groupFoodSum[groupId][i]);
            clerk->condition[1]->Signal(clerk->lock);
            // wait for acknowledgement and amount
            clerk->condition[1]->Wait(clerk->lock);
        }

 
        // TODO: give money
        double amount = clerk->getAmount();
        clerk->setPayment(amount);
        printf("Customer [%d] in Group [%d] in ConcessionClerk line [%d] is paying [%.2f] for food\n",customerId,groupId,lineIndex,amount);
        clerk->condition[1]->Signal(clerk->lock);
        //wait for clerk to acknowledge money 
        clerk->condition[1]->Wait(clerk->lock);
        // customer leave, get next customer
        clerk->condition[1]->Signal(clerk->lock);
        printf("Customer [%d] in Group [%d] is leaving ConcessionClerk[%d]\n",customerId,groupId,lineIndex);

    } else {
        printf("Customer [%d] in Group [%d] sees ConcessionClerk [%d] is on break.\n",customerId,groupId,lineIndex);
        clerk->subWaitingSize();
        // for race condition of waiting size
        lBuyFood->Release();
        clerk->lock->Release();
        buyFood();
        return;
    }
    clerk->lock->Release();

}

bool Customer::countFood() {
    if (getIsTicketBuyer()) {
        lGroup[groupId]->Acquire();
        // if self want food
        for (int i = 0;i < 2; ++i) {
            groupFoodSum[groupId][i] += answerForFood(i);
        }
        // ask group mate if they want food
        cGroupFood[groupId]->Broadcast(lGroup[groupId]);
        lGroup[groupId]->Release();
        //sGroupFood[groupId]->V();
        // wait for all the customer answer for food
        for (int i = 1;i < groupSize; ++i) {
            sGroupFood[groupId]->P();
        }

        printf("Customer [%d] in Group [%d] has [%d] popcorn and [%d] soda request from a group member\n",customerId,groupId,groupFoodSum[groupId][0],groupFoodSum[groupId][1]);
        if (!groupFoodSum[groupId][0] && !groupFoodSum[groupId][1]) {
            groupFood[groupId] = true;
            return false;
        }
        return true;
    }
    return false;
}
// rand for food
int Customer::answerForFood(int food) {
    int choice = rand()%4;
    return (rand()%4) > 0;
}

void Customer::checkTickets() {
    int lineIndex = -1;
    while (lineIndex == -1) {
        lineIndex = getInLine(lCheckTickets, (Employee**)tt, MAX_TT);
    }
    printf("Customer [%d] in Group [%d] is getting in TicketTaker line [%d]\n",customerId,groupId,lineIndex);
    TicketTaker *clerk = tt[lineIndex];
    if (clerk->getIsBusy()) {
        clerk->condition[0]->Wait(lCheckTickets);
    }
    printf("Customer [%d] in Group [%d] is walking up to TicketTaker[%d] to give [numberOfTickets] tickets.\n",customerId,groupId,lineIndex);
    // interact with TicketTaker 
    clerk->lock->Acquire();
    if (!clerk->getIsBreak()) {
        lCheckTickets->Release();    
        // give ticket receipt 
        clerk->setTicketSum(groupTicketSum[groupId]);
        clerk->condition[1]->Signal(clerk->lock);

        clerk->condition[1]->Wait(clerk->lock);
        
        // get into room
        clerk->condition[1]->Signal(clerk->lock);
    printf("Customer [%d] in Group [%d] is leaving TicketTaker[%d]\n",customerId,groupId,lineIndex);
    } else {
    printf("Customer [%d] in Group [%d] sees TicketTaker [%d] is on break.\n",customerId,groupId,lineIndex);
        clerk->subWaitingSize();
        // for race condition of waiting size
        lCheckTickets->Release();
        clerk->lock->Release();
        checkTickets();
        return;
    }

    clerk->lock->Release();
}

void Customer::goBathroom() {
    ;
}
void Customer::leaveTheater() {
}
void Customer::arrangeSeats() {
    ;
}
void Customer::waitTickets() {
    lGroup[groupId]->Acquire();
    // if ticketbuyer had broadcast, do not wait
    if (!groupTicket[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    // after get ticket, semaphore
    sGroup[groupId]->V();
    lGroup[groupId]->Release();
}
// regular customer wait for buyFood
void Customer::waitFood() {
    lGroup[groupId]->Acquire();
    // wait for ticketbuyer to ask for if need food
    if (!groupAskForFood[groupId]) {
        cGroupFood[groupId]->Wait(lGroup[groupId]);
    }
    for (int i = 0;i < 2; ++i) {
        groupFoodSum[groupId][i] += answerForFood(i);
    }
    sGroupFood[groupId]->V();
    // if ticketbuyer had broadcast, do not wait
    if (!groupFood[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    // after get food, semaphore
    sGroup[groupId]->V();
    lGroup[groupId]->Release();
}
void Customer::waitSeats() {
    lGroup[groupId]->Acquire();
    // if ticketbuyer had broadcast, do not wait
    if (!groupFood[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }

    // take seat
    lStartMovie->Acquire();
    // get seat number from ticketbuyer
    //seatState[] = ture;
    //seatNumber = 
    sGroup[groupId]->V();
    lStartMovie->Release();

    // after get seat, semaphore
    sGroup[groupId]->V();
    lGroup[groupId]->Release();
}

void Customer::printTCStatus(){

    //Print the number of waiting customer in each clerk's line

    printf("TC State:");
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

    printf("TC WaitLine(#):");

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
