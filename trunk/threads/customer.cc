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

/*void Customer::waitGroup() {
    for (int i = 1;i < groupSize; ++i) {
        sGroup[groupId]->P();
    }
}*/
void Customer::action() {
    if (getIsTicketBuyer()) {
        // ticket buyer
        DEBUG('z', "\tGroup [%d] start buy tickets.\n", groupId);
        buyTickets();
        DEBUG('z', "\tGroup [%d] finish buy tickets.\n", groupId);
        proceed(groupTicket);
//        DEBUG('z', "\tGroup [%d] start waitGroup after finish buy tickets.\n", groupId);
//        waitGroup();
//        DEBUG('z', "\tGroup [%d] finish waitGroup after finish buy tickets.\n", groupId);
        if (countFood()) {
            DEBUG('z', "\tGroup [%d] start buy food.\n", groupId);
            buyFood();
            DEBUG('z', "\tGroup [%d] finish buy food.\n", groupId);
        }
        proceed(groupFood);
//        DEBUG('z', "\tGroup [%d] start waitGroup after finish buy food.\n", groupId);
//        waitGroup();
//       DEBUG('z', "\tGroup [%d] finish waitGroup after finish buy food.\n", groupId);
        DEBUG('z', "\tGroup [%d] start check tickets.\n", groupId);
        checkTickets();
        DEBUG('z', "\tGroup [%d] finish check tickets.\n", groupId);
        proceed(groupSeat);
//        DEBUG('z', "\tGroup [%d] start waitGroup after finish check tickets.\n", groupId);
//        waitGroup();
//        DEBUG('z', "\tGroup [%d] finish waitGroup after finish check tickets.\n", groupId);
        DEBUG('z', "\tGroup [%d] start take seat.\n", groupId);
        arrangeSeats();
        DEBUG('z', "\tGroup [%d] finish take seat.\n", groupId);
    } else {
        // regular customer
        waitTickets();
        waitFood();
        waitCheck();
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
        DEBUG('z', "\tbusy%d\n", employee[lineIndex]->getIsBusy()?1:0);
        if(employee[lineIndex]->getIsBusy()){        
            employee[lineIndex]->addWaitingSize();
        }
    }
    DEBUG('z', "\t%dline%d\n", lineIndex, employee[lineIndex]->getWaitingSize());
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
        clerk->setIsBusy(true);
        lBuyTickets->Release(); 
        // tell clerk tickets sum
        clerk->setGroupId(groupId);
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
        clerk->setIsBusy(true);
        lBuyFood->Release();
 
        clerk->setGroupId(groupId);
        // tell clerk popcorn sum and soda sum

        for (int i = 0;i < 2; ++i) {
            clerk->setFood(i, groupFoodSum[groupId][i]);
            clerk->condition[1]->Signal(clerk->lock);
            // wait for acknowledgement and amount
            clerk->condition[1]->Wait(clerk->lock);
        }

 
        // give money
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
        // set monitor variable true
        groupAskForFood[groupId] = true;
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
    printf("Customer [%d] in Group [%d] is walking up to TicketTaker[%d] to give [%d] tickets.\n",customerId,groupId,lineIndex, ticketReceipt[groupId]);
    // interact with TicketTaker 
    clerk->lock->Acquire();
    if (!clerk->getIsBreak()) {
        clerk->setIsBusy(true);
        lCheckTickets->Release();
        clerk->setGroupId(groupId);
        // give ticket receipt 
        clerk->setTicketSum(ticketReceipt[groupId]);
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
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
    // if ticketbuyer had broadcast, do not wait
    if (!groupTicket[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    // after get ticket
    printf("Customer [%d] of group [%d] has been told by the HeadCustomer to proceed.\n", customerId, groupId);
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
    // has chosen food
    sGroupFood[groupId]->V();
    // wait for proceed
    sGroup[groupId]->V();
    // if ticketbuyer had broadcast, do not wait
    if (!groupFood[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    // after get food
    printf("Customer [%d] of group [%d] has been told by the HeadCustomer to proceed.\n", customerId, groupId);
    lGroup[groupId]->Release();
}
void Customer::waitCheck() {
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
    // if ticketbuyer had broadcast, do not wait
    if (!groupSeat[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }

    printf("Customer [%d] of group [%d] has been told by the HeadCustomer to proceed.\n", customerId, groupId);
    // take seat
    //lStartMovie->Acquire();
    // get seat number from ticketbuyer
    //seatState[] = ture;
    //seatNumber = 
    //sGroup[groupId]->V();
    //lStartMovie->Release();

    // after get seat, semaphore
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
void Customer::proceed(bool *flag) {
    for (int i = 1;i < groupSize; ++i) {
        sGroup[groupId]->P();
    }

    // tell group to go
    lGroup[groupId]->Acquire();
    cGroup[groupId]->Broadcast(lGroup[groupId]);
    printf("HeadCustomer [%d] of group [%d] has told the group to proceed.\n", customerId, groupId);
    // monitor variables
    flag[groupId] = true;
    lGroup[groupId]->Release();
}
