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
    //printf("HeadCustomer [%d] of group [%d] is waiting for the group to form.\n",customerId,groupId);
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
        DEBUGINFO('c', "Customer [%d] in group [%d] finish waitGroup after finish buy tickets", customerId, groupId);
        if (countFood()) {
            DEBUGINFO('c', "Customer [%d] in group [%d] start buy food", customerId, groupId);
            buyFood();
        }
        DEBUGINFO('c', "Customer [%d] in group [%d] finish buy food", customerId, groupId);
        proceed(groupFood);
//        DEBUG('z', "\tGroup [%d] start waitGroup after finish buy food.\n", groupId);
//        waitGroup();
//       DEBUG('z', "\tGroup [%d] finish waitGroup after finish buy food.\n", groupId);
        DEBUGINFO('c', "Customer [%d] in group [%d] start check tickets.", customerId, groupId);
        DEBUG('z', "\tGroup [%d] start \n", groupId);
        
        checkTickets();
        DEBUGINFO('c', "Customer [%d] in group [%d] finish check Ticket", customerId, groupId);
        DEBUG('z', "\tGroup [%d] finish check tickets.\n", groupId);
        proceed(groupSeat);
//        DEBUG('z', "\tGroup [%d] start waitGroup after finish check tickets.\n", groupId);
//        waitGroup();
//        DEBUG('z', "\tGroup [%d] finish waitGroup after finish check tickets.\n", groupId);
        DEBUGINFO('c', "Customer [%d] in group [%d] start take seat", customerId, groupId);
        DEBUG('z', "\tGroup [%d] start take seat.\n", groupId);
        arrangeSeats();
        DEBUGINFO('c', "Customer [%d] in group [%d] finish take seat", customerId, groupId);
        DEBUG('z', "\tGroup [%d] finish take seat.\n", groupId);
        watchMovie();
        DEBUGINFO('c', "Customer [%d] in group [%d] finish watch Movie", customerId, groupId);
        proceed(groupLeaveRoom);
        prtLeaveRoomMsg();
        DEBUGINFO('c', "Customer [%d] in group [%d] leave theater room", customerId, groupId);
        checkBathroom();
        DEBUGINFO('c', "Customer [%d] in group [%d] leave bath room", customerId, groupId);
        proceed(groupLeaveTheater);
        leaveTheater(); 
        DEBUGINFO('c', "Customer [%d] in group [%d] leave theater", customerId, groupId);
    } else {
        // regular customer
        DEBUGINFO('c', "Customer [%d] in group [%d] waitTickets", customerId, groupId);
        waitTickets();
        DEBUGINFO('c', "Customer [%d] in group [%d] waitFood", customerId, groupId);
        waitFood();  
        DEBUGINFO('c', "Customer [%d] in group [%d] waitCheck", customerId, groupId);
        waitCheck();   
        DEBUGINFO('c', "Customer [%d] in group [%d] waitSeats", customerId, groupId);        
        waitSeats();
        DEBUGINFO('c', "Customer [%d] in group [%d] watchMovie", customerId, groupId);
        watchMovie();
        DEBUGINFO('c', "Customer [%d] in group [%d] waitLeaveRoom", customerId, groupId);
        waitLeaveRoom();
        prtLeaveRoomMsg();
        DEBUGINFO('c', "Customer [%d] in group [%d] waitBathroom", customerId, groupId);
        waitBathroom();
        DEBUGINFO('c', "Customer [%d] in group [%d] waitLeaveTheater", customerId, groupId);
        waitLeaveTheater();
        leaveTheater(); 
        DEBUGINFO('c', "Customer [%d] in group [%d] leave theater", customerId, groupId);
    }
}
bool Customer::getIsTicketBuyer() {
    return isTicketBuyer;
}

int Customer::getInLine(Lock *lock, Condition *cNoClerk, int count, bool noClerk, Employee** employee ) {
    int i, j;
    int lineIndex = -1;
    int lineLen = -1;
    lock->Acquire();

    DEBUG('z', "\tGroup [%d] start find line\n", groupId);    
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
    DEBUG('z', "\tGroup [%d] finish find line\n", groupId);    
    // if no service
    if (lineIndex == -1) {
        // need to wait until the manager signal them
        noClerk = true;
        printf("Customer [%d] in Group [%d] is in the lobby.\n", customerId, groupId);
        cNoClerk->Wait(lock);
        printf("Customer [%d] in Group [%d] is leaving the lobby.\n",  customerId, groupId);
    } else {
        DEBUG('z', "\tbusy%d\n", employee[lineIndex]->getIsBusy()?1:0);
        employee[lineIndex]->addWaitingSize();
    }
    DEBUG('z', "\t%dline%d\n", lineIndex, employee[lineIndex]->getWaitingSize());
    DEBUGINFO('c', "\tCustomer [%d] in Group [%d] get in %s line [%d], length:%d\n", customerId, groupId, employee[lineIndex]->getEmployeeType(), lineIndex, employee[lineIndex]->getWaitingSize());
    return lineIndex; 
}
void Customer::buyTickets() {

    int lineIndex = -1;

    //lBuyTickets->Acquire();
    while (lineIndex == -1) {
        lineIndex = getInLine(lBuyTickets, cNoTicketClerk, MAX_TC, noTicketClerk, (Employee**)tc);        
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
    clerk->subWaitingSize();
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
        // for race condition of waiting size
        //clerk->subWaitingSize();
        lBuyTickets->Release();
        clerk->lock->Release();
        buyTickets();
        return;
    }
    clerk->lock->Release();
    DEBUGINFO('c', "Customer [%d] in group [%d] finish buy ticket from TicketClerk [%d]", customerId, groupId, lineIndex);
}

void Customer::buyFood() {
    int lineIndex = -1;
    //lBuyFood->Acquire();
    while (lineIndex == -1) {
        lineIndex = getInLine(lBuyFood, cNoConcessionClerk, MAX_CC, noConcessionClerk, (Employee**)cc);
    }
    DEBUGINFO('c', "Customer [%d] in group [%d] get in ConcessionClerk [%d]", customerId, groupId, lineIndex);
    printf("Customer [%d] in Group [%d] is getting in ConcessionClerk line [%d]\n",customerId,groupId,lineIndex);

    ConcessionClerk *clerk = cc[lineIndex];
    if (clerk->getIsBusy()) {
        clerk->condition[0]->Wait(lBuyFood);
    }
    DEBUGINFO('c', "Customer [%d] in group [%d] is walking up to ConcessionClerk [%d]", customerId, groupId, lineIndex);
    printf("Customer [%d] in Group [%d] is walking up to ConcessionClerk[%d] to buy [%d] popcorn and [%d] soda.\n",customerId,groupId,lineIndex,groupFoodSum[groupId][0],groupFoodSum[groupId][1]);
    // interact with ConcessionClerk 
    clerk->lock->Acquire();
    clerk->subWaitingSize();
    if (!clerk->getIsBreak()) {
        clerk->setIsBusy(true);
        lBuyFood->Release();
 
        clerk->setGroupId(groupId);
        // tell clerk popcorn sum and soda sum

        DEBUGINFO('c', "Customer [%d] in group [%d] interact with ConcessionClerk [%d]", customerId, groupId, lineIndex);
        for (int i = 0;i < 2; ++i) {
            clerk->setFood(i, groupFoodSum[groupId][i]);
            clerk->condition[1]->Signal(clerk->lock);
            // wait for acknowledgement and amount
            clerk->condition[1]->Wait(clerk->lock);
        }

 
        DEBUGINFO('c', "Customer [%d] in group [%d] is paying to ConcessionClerk [%d]", customerId, groupId, lineIndex);
        // give money
        double amount = clerk->getAmount();
        clerk->setPayment(amount);
        printf("Customer [%d] in Group [%d] in ConcessionClerk line [%d] is paying [%.2f] for food\n",customerId,groupId,lineIndex,amount);
        clerk->condition[1]->Signal(clerk->lock);
        //wait for clerk to acknowledge money 
        clerk->condition[1]->Wait(clerk->lock);
        // customer leave, get next customer
        clerk->condition[1]->Signal(clerk->lock);

        DEBUGINFO('c', "Customer [%d] in group [%d] is leaving ConcessionClerk [%d]", customerId, groupId, lineIndex);
        printf("Customer [%d] in Group [%d] is leaving ConcessionClerk[%d]\n",customerId,groupId,lineIndex);

    } else {
        printf("Customer [%d] in Group [%d] sees ConcessionClerk [%d] is on break.\n",customerId,groupId,lineIndex);
        DEBUGINFO('c', "Customer [%d] in group [%d] sees ConcessionClerk [%d] on break", customerId, groupId, lineIndex);
        //clerk->subWaitingSize();
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
        lineIndex = getInLine(lCheckTickets, cNoTicketTaker, MAX_TT, noTicketTaker, (Employee**)tt);
    }
    
    printf("Customer [%d] in Group [%d] is getting in TicketTaker line [%d]\n",customerId,groupId,lineIndex);
    TicketTaker *clerk = tt[lineIndex];
    DEBUGINFO('c', "Customer [%d] in group [%d] get in %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
    // ? race condition
    if (clerk->getIsBusy()) {
        DEBUGINFO('c', "Customer [%d] in group [%d] wait for TicketTaker [%d]", customerId, groupId, lineIndex);
        clerk->condition[0]->Wait(lCheckTickets);
        
    }

    // if stop ticket taken
    lTicketTaken->Acquire();
    clerk->subWaitingSize();
    if (stopTicketTaken) {
        DEBUGINFO('c', "Customer [%d] in group [%d] sees in TicketTaker line [%d] no longer taking tickets", customerId, groupId, lineIndex);
        // TODO: ask all group to goto lobby
        printf("Customer [%d] in Group [%d] sees TicketTaker [%d] is no longer taking tickets. Going to the lobby.\n", customerId, groupId, lineIndex);
        printf("Customer [%d] in Group [%d] is in the lobby.\n", customerId, groupId);
        lCheckTickets->Release();
        cTicketTaken->Wait(lTicketTaken);
        DEBUGINFO('c', "Customer [%d] in group [%d] leave lobby to TicketTaker", customerId, groupId);
        printf("Customer [%d] in Group [%d] is leaving the lobby.\n",  customerId, groupId);
        lTicketTaken->Release();
        checkTickets();
        return;
    }
    lTicketTaken->Release();

    DEBUGINFO('c', "Customer [%d] in Group [%d] is walking up to %s [%d] to give [%d] tickets.\n",customerId,groupId, clerk->getEmployeeType(), lineIndex, ticketReceipt[groupId]);
    printf("Customer [%d] in Group [%d] is walking up to TicketTaker[%d] to give [%d] tickets.\n",customerId,groupId,lineIndex, ticketReceipt[groupId]);
    // interact with TicketTaker 
    clerk->lock->Acquire();
    if (!clerk->getIsBreak() ) {
        DEBUGINFO('c', "Customer [%d] in group [%d] get lock with %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        clerk->setIsBusy(true);
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        lCheckTickets->Release();
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        clerk->setGroupId(groupId);
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        // give ticket receipt 
        clerk->setTicketSum(ticketReceipt[groupId]);
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        clerk->condition[1]->Signal(clerk->lock);
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        clerk->condition[1]->Wait(clerk->lock);
        DEBUGINFO('c', "Customer [%d] in group [%d] get lock with %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        
        if (stopTicketTaken) {
            // TODO: ask all group to go back lobby
            clerk->lock->Release();
            printf("Customer [%d] in Group [%d] sees TicketTaker [%d] is no longer taking tickets. Going to the lobby.", customerId, groupId, lineIndex);
            printf("Customer [%d] in Group [%d] is in the lobby.\n", customerId, groupId);
            DEBUGINFO('c', "Customer [%d] in group [%d] wait to retake ticket", customerId, groupId, lineIndex);
            lTicketTaken->Acquire();
            cTicketTaken->Wait(lTicketTaken);
            printf("Customer [%d] in Group [%d] is leaving the lobby.\n",  customerId, groupId);
            DEBUGINFO('c', "Customer [%d] in group [%d] get lock with ticket taker [%d]", customerId, groupId, lineIndex);
            
            lTicketTaken->Release();
            checkTickets();
            return;
        }
        DEBUGINFO('c', "Customer [%d] in group [%d] is leaving %s [%d] get into room", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        // get into room
        clerk->condition[1]->Signal(clerk->lock);
        printf("Customer [%d] in Group [%d] is leaving TicketTaker[%d]\n",customerId,groupId,lineIndex);
DEBUGINFO('c', "Customer [%d] in group [%d] is leaving %s [%d] after get into room", customerId, groupId, clerk->getEmployeeType(), lineIndex);
    } else {
        DEBUGINFO('c', "Customer [%d] in group [%d] sees TicketTaker [%d] is on break.", customerId, groupId, lineIndex);
        printf("Customer [%d] in Group [%d] sees TicketTaker [%d] is on break.\n",customerId,groupId,lineIndex);
        lCheckTickets->Acquire();
        // for race condition of waiting size
        lCheckTickets->Release();
        clerk->lock->Release();
        checkTickets();
        return;
    }
DEBUGINFO('c', "Customer [%d] in group [%d] is leaving %s [%d] b get into room", customerId, groupId, clerk->getEmployeeType(), lineIndex);
    clerk->lock->Release();
DEBUGINFO('c', "Customer [%d] in group [%d] is leaving %s [%d] c get into room", customerId, groupId, clerk->getEmployeeType(), lineIndex);
}

// rand for food
bool Customer::answerForBathroom() {
    
    int chance = rand()%100;
    if(chance<=25){
        return true;
    }
    return false;
}


void Customer::goBathroom() {
        
      int i=0;
     for (i = 0; i < 100; i++) {
            currentThread->Yield();
     }



}
void Customer::leaveTheater() {
    // if leave, --customerLeft
    lCustomerLeft->Acquire(); 
    --customerLeft;
    lCustomerLeft->Release();
    printf("Customer [%d] in Group [%d] has left the movie theater\n",customerId,groupId); 
}
void Customer::arrangeSeats() {
    
    lFindSeats->Acquire();

    int i=0;
    // Head customer get seat for self
         
    getSeats(groupSize);
    seatPos=SeatLocation[0];	
    printf("Customer [%d] in Group [%d] has found the following seat: row [%d] and seat [%d]\n",customerId,groupId,(seatPos/MAX_ROW),seatPos%MAX_COL);   
    lGroup[groupId]->Acquire();
    DEBUGINFO('c', "Customer [%d] in Group [%d] arrange seat %d", customerId,groupId,SeatLocation[0]);
    seatState[SeatLocation[0]]=true;

    //Head customer arrange seats for group member
    for(i=1;i< groupSize;i++){
        DEBUGINFO('c', "Customer [%d] in Group [%d] arrange seat %d", customerId,groupId,SeatLocation[i]);
        seatPos=SeatLocation[i];
        sWaitSeat[groupId]->V();        
        cGroup[groupId]->Wait(lGroup[groupId]);
        seatState[seatPos]=true;
    }
    lGroup[groupId]->Release();
    lFindSeats->Release();      

}
int Customer::getSeats(int Size){
    
    int i=0,j=0;
    int consecutive=0,RequestSeat=0;
    int SeatAvailable[MAX_ROW]={0};

    bool bFind=false;
    
    //assumption:customer always choose seat from the beginning of a row
    //
    for(i=0;i<MAX_ROW;i++){
        consecutive=0;
        for(j=0;j<MAX_COL;j++){
            if(seatState[j+i*MAX_COL]==false){
                consecutive++;         
            }
        }
        SeatAvailable[i]=consecutive; 
    }

    //Search for the first time if any consecutive seats
    for(i=0;i<MAX_ROW;i++){  
        if(SeatAvailable[i]>=Size){
            for(j=0;j<Size;j++){
                SeatLocation[j]=(MAX_COL-SeatAvailable[i]+j)+i*MAX_COL;               
                bFind=true;
            }
            SeatAvailable[i]=0;
            break;
        }
    }

     //Search for any available seats if no consecutive seats
    if(bFind==false){
        for(i=0;i<MAX_ROW;i++){  
            if(SeatAvailable[i]!=0){
                  for(j=0;j<SeatAvailable[i];j++){
                        SeatLocation[RequestSeat]=(MAX_COL-SeatAvailable[i])+i*MAX_COL; 
                        RequestSeat++;          
                  }
                  SeatAvailable[i]=0;
            }
            if(RequestSeat==Size-1){
                break;
            }
        }   
    }

  
    return 1;
}

void Customer::waitTickets() {
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
    // if ticketbuyer had broadcast, do not wait
    printf("Customer [%d] of group [%d] is waiting for the HeadCustomer.\n",customerId,groupId);
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
    printf("Customer [%d] of group [%d] is waiting for the HeadCustomer.\n",customerId,groupId);
    if (!groupFood[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    // after get food
    printf("Customer [%d] of group [%d] has been told by the HeadCustomer to proceed.\n", customerId, groupId);
    lGroup[groupId]->Release();
}
void Customer::waitCheck() {
    DEBUGINFO('c', "Customer [%d] in group [%d] wait check a", customerId, groupId);
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
    DEBUGINFO('c', "Customer [%d] in group [%d] wait check b", customerId, groupId);
    // TODO: wait in the lobby 
    printf("Customer [%d] of group [%d] is waiting for the HeadCustomer.\n",customerId,groupId);
    if (!groupSeat[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    DEBUGINFO('c', "Customer [%d] in group [%d] has been told by the HeadCustomer to proceed.", customerId, groupId);
    printf("Customer [%d] of group [%d] has been told by the HeadCustomer to proceed.\n", customerId, groupId);
   
 
    lGroup[groupId]->Release();

}
void Customer::waitSeats() {
 
    
    sWaitSeat[groupId]->P();
  
    lGroup[groupId]->Acquire();
    printf("Customer [%d] in Group [%d] has found the following seat: row [%d] and seat [%d]\n",customerId,groupId,(seatPos/MAX_ROW),(seatPos%MAX_COL));   
    cGroup[groupId]->Signal(lGroup[groupId]);
    lGroup[groupId]->Release();
       
        
}


void Customer::watchMovie(){
    DEBUGINFO('c', "Customer [%d] in group [%d] is waiting for sMT_CR_Chec P.", customerId, groupId);
    //Customers are checkd by Movie Techinician if they were seated
    sMT_CR_Check->P();
    DEBUGINFO('c', "Customer [%d] in group [%d] is sitting in a theater room seat.", customerId, groupId);
    lStartMovie->Acquire();
    DEBUGINFO('c', "Customer [%d] in group [%d] is sitting in a theater room seat.", customerId, groupId);
    cMT_CR_Check->Signal(lStartMovie);
    DEBUGINFO('c', "Customer [%d] in group [%d] is sitting in a theater room seat.", customerId, groupId);
    lStartMovie->Release();
    DEBUGINFO('c', "Customer [%d] in group [%d] is sitting in a theater room seat and wait for sMT_CR_Stop.", customerId, groupId);
    printf("Customer [%d] in group [%d] is sitting in a theater room seat.\n",customerId,groupId);
 DEBUGINFO('c', "%s P() %s value %d", currentThread->getName(), getName(), value);
    //watching movie and wait Movie Techinician to wake them up
    sMT_CR_Stop->P();
    DEBUGINFO('c', "Customer [%d] in group [%d] acquire lStopMovie, lStopMovie's owner : %s", customerId,groupId, lStopMovie->getOwnerThread() == NULL? "NULL": lStopMovie->getOwnerThread()->getName());
    lStopMovie->Acquire();
    DEBUGINFO('c', "Customer [%d] in group [%d] end acquire lStopMovie, lStopMovie's owner : %s", customerId,groupId, lStopMovie->getOwnerThread() == NULL? "NULL": lStopMovie->getOwnerThread()->getName());
    cMT_CR_Stop->Signal(lStopMovie);
    lStopMovie->Release();
    DEBUGINFO('c', "Customer [%d] in group [%d] is watching movie and wait up by Movie Techinician.", customerId, groupId);

}
void Customer::waitLeaveRoom(){

    //regroup 
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
    if (!groupLeaveRoom[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    lGroup[groupId]->Release();

}
void Customer::checkBathroom(){


    lGroup[groupId]->Acquire();
    // ask group mate if they want to go bathroom
    cGroup[groupId]->Broadcast(lGroup[groupId]);
        // set monitor variable true
        groupAskForBathroom[groupId] = true;
        lGroup[groupId]->Release();

         // wait for all the customer answer for bathroom
        for (int i = 1;i < groupSize; ++i) {
            sGroupBathroom[groupId]->P();
        }

        if(answerForBathroom()){
        	 printf("Customer [%d] in Group [%d] is going to the bathroom.\n",customerId,groupId);
             //goBathroom(); 
             printf("Customer [%d] in Group [%d] is leaving the bathroom.\n",customerId,groupId);
             printf("Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId); 
    	}else{
        	 printf("Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId); 
    	}
       
        
    for (int i = 1;i < groupBathroomSum[groupId]; ++i) {
        sGroupLeaveBathroom[groupId]->P();
    }

}
void Customer::waitBathroom(){

    lGroup[groupId]->Acquire();
    // wait for ticketbuyer to ask for if need food
    if (!groupAskForBathroom[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }

    lGroup[groupId]->Release();

    if(answerForBathroom()){
         groupBathroomSum[groupId]++;
         printf("Customer [%d] in Group [%d] is going to the bathroom.\n",customerId,groupId); 
         //goBathroom(); 
         sGroupBathroom[groupId]->V();
         printf("Customer [%d] in Group [%d] is leaving the bathroom.\n",customerId,groupId);
         sGroupLeaveBathroom[groupId]->V();
         printf("Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId);   
    }else{
         printf("Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId); 

        sGroupBathroom[groupId]->V();
    }


  
    
}

void Customer::prtLeaveRoomMsg(){

    printf("Customer [%d] in group [%d] is getting out of a theater room seat.\n",customerId,groupId);
}
void Customer::waitLeaveTheater(){

    //regroup 
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
    if (!groupLeaveTheater[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    lGroup[groupId]->Release();

}

void Customer::proceed(bool *flag) {
    DEBUGINFO('c', "Customer [%d] in Group [%d] proceed to next.\n",customerId,groupId);
    for (int i = 1;i < groupSize; ++i) {
        sGroup[groupId]->P();
    }
    DEBUGINFO('c', "Customer [%d] in Group [%d] proceed to next.\n",customerId,groupId);
    // tell group to go
    DEBUGINFO('c', "Customer [%d] in Group [%d] acquire lGroup[%d], lGroup[]'s owner is %s ", customerId, groupId, groupId, lGroup[groupId]->getOwnerThread() == NULL? "NULL" : lGroup[groupId]->getOwnerThread()->getName());
    lGroup[groupId]->Acquire();
    cGroup[groupId]->Broadcast(lGroup[groupId]);
    printf("HeadCustomer [%d] of group [%d] has told the group to proceed.\n", customerId, groupId);
    // monitor variables
    flag[groupId] = true;
    lGroup[groupId]->Release();
}
int Customer::getGroupId(){

    return   groupId;
}
int Customer::getId(){

    return   customerId;

}
