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
       /* proceed(groupTicket);
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
        watchMovie();
        proceed(groupLeaveRoom);
        prtLeaveRoomMsg();
        checkBathroom();
        proceed(groupLeaveTheater);
        leaveTheater(); */
    } else {
        // regular customer
        waitTickets();
        /*waitFood();
        waitCheck();
        waitSeats();
        watchMovie();
        waitLeaveRoom();
        prtLeaveRoomMsg();
        waitBathroom();
        waitLeaveTheater();
        leaveTheater(); */
    }
}
bool Customer::getIsTicketBuyer() {
    return isTicketBuyer;
}

int Customer::getInLine(Lock *lock, Condition *cNoClerk, int count, bool noClerk, Employee** employee ) {
    int i, j;
    int lineIndex = -1;
    int lineLen = -1;
    //lock->Acquire();

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
    lBuyTickets->Acquire();
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
    printf("\t%s DEBUG error 1\n", currentThread->getName());
    clerk->lock->Acquire();
    printf("\t%s DEBUG error 2\n", currentThread->getName());
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
    lBuyFood->Acquire();
    while (lineIndex == -1) {
        lineIndex = getInLine(lBuyFood, cNoConcessionClerk, MAX_CC, noConcessionClerk, (Employee**)cc);
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
    lCheckTickets->Acquire();
    int lineIndex = -1;
    while (lineIndex == -1) {
        lineIndex = getInLine(lCheckTickets, cNoTicketTaker, MAX_TT, noTicketTaker, (Employee**)tt);
    }
    printf("Customer [%d] in Group [%d] is getting in TicketTaker line [%d]\n",customerId,groupId,lineIndex);
    TicketTaker *clerk = tt[lineIndex];
    // ? race condition
    if (clerk->getIsBusy() && !stopTicketTaken) {
        clerk->condition[0]->Wait(lCheckTickets);
    }
    // if stop ticket taken
    lTicketTaken->Acquire();
    if (stopTicketTaken) {
        // TODO: ask all group to goto lobby
        printf("Customer [%d] in Group [%d] sees TicketTaker [%d] is no longer taking tickets. Going to the lobby.\n", customerId, groupId, lineIndex); 
        clerk->subWaitingSize();
        printf("Customer [%d] in Group [%d] is in the lobby.\n", customerId, groupId);
        cTicketTaken->Wait(lTicketTaken);
        printf("Customer [%d] in Group [%d] is leaving the lobby.\n",  customerId, groupId);
        lTicketTaken->Release();
        lCheckTickets->Release();
        checkTickets();
        return;
    }
    lTicketTaken->Release();

    printf("Customer [%d] in Group [%d] is walking up to TicketTaker[%d] to give [%d] tickets.\n",customerId,groupId,lineIndex, ticketReceipt[groupId]);
    // interact with TicketTaker 
    clerk->lock->Acquire();
    if (!clerk->getIsBreak() ) {
        clerk->setIsBusy(true);
        lCheckTickets->Release();
        clerk->setGroupId(groupId);
        // give ticket receipt 
        clerk->setTicketSum(ticketReceipt[groupId]);
        clerk->condition[1]->Signal(clerk->lock);

        clerk->condition[1]->Wait(clerk->lock);
        lTicketTaken->Acquire();
        if (stopTicketTaken) {
            // TODO: ask all group to go back lobby
            lCheckTickets->Acquire();
            clerk->lock->Release();
            lTicketTaken->Release();
            // leave the waiting line
            clerk->subWaitingSize();
            printf("Customer [%d] in Group [%d] sees TicketTaker [%d] is no longer taking tickets. Going to the lobby.", customerId, groupId, lineIndex);
            printf("Customer [%d] in Group [%d] is in the lobby.\n", customerId, groupId);
            cTicketTaken->Wait(lCheckTickets);
            printf("Customer [%d] in Group [%d] is leaving the lobby.\n",  customerId, groupId);
            lCheckTickets->Release();
            checkTickets();
            return;
        }
        lTicketTaken->Release();
        // get into room
        clerk->condition[1]->Signal(clerk->lock);
        printf("Customer [%d] in Group [%d] is leaving TicketTaker[%d]\n",customerId,groupId,lineIndex);
    } else {
        printf("Customer [%d] in Group [%d] sees TicketTaker [%d] is on break.\n",customerId,groupId,lineIndex);
        lCheckTickets->Acquire();
        clerk->subWaitingSize();
        // for race condition of waiting size
        lCheckTickets->Release();
        clerk->lock->Release();
        checkTickets();
        return;
    }

    clerk->lock->Release();
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
    seatState[SeatLocation[0]]=true;
    for(i=1;i< groupSize;i++){
        
        seatPos=SeatLocation[i];
        //groupArrangeSeat[groupId]=true;	
        sWaitSeat[groupId]->V();        
        cGroup[groupId]->Wait(lGroup[groupId]);
        seatState[seatPos]=true;
        //groupArrangeSeat[groupId]=false;	
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
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();

    // TODO: wait in the lobby 
    printf("Customer [%d] of group [%d] is waiting for the HeadCustomer.\n",customerId,groupId);
    if (!groupSeat[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    printf("Customer [%d] of group [%d] has been told by the HeadCustomer to proceed.\n", customerId, groupId);
   
    //TODO:delete
    /*
     lStartMovie->Acquire();
    // get seat number from ticketbuyer
    //seatState[] = ture;
    //seatNumber = 
    sGroup[groupId]->V();
    lStartMovie->Release();
   */
    // after get seat, semaphore
    lGroup[groupId]->Release();

}
void Customer::waitSeats() {
 
    //if(!groupArrangeSeat[groupId]){
        sWaitSeat[groupId]->P();
   // }
    lGroup[groupId]->Acquire();
    printf("Customer [%d] in Group [%d] has found the following seat: row [%d] and seat [%d]\n",customerId,groupId,(seatPos/MAX_ROW),(seatPos%MAX_COL));   
    cGroup[groupId]->Signal(lGroup[groupId]);
    lGroup[groupId]->Release();
       
        
     
    //TODO:to delete
//    printf("Customer [%d] of group [%d] has been told by the HeadCustomer to proceed.\n", customerId, groupId);
    // take seat
    /*lStartMovie->Acquire();
    // get seat number from ticketbuyer
    //seatState[] = ture;
    //seatNumber = 
    //sGroup[groupId]->V();
    //lStartMovie->Release();

    // after get seat, semaphore
    sGroup[groupId]->V();
    lGroup[groupId]->Release();

  */
}


void Customer::watchMovie(){
   
    //Customers are checkd by Movie Techinician if they were seated
    sMT_CR_Check->P();
    lStartMovie->Acquire();
    cMT_CR_Check->Signal(lStartMovie);
    lStartMovie->Release();
    printf("Customer [%d] in group [%d] is sitting in a theater room seat.\n",customerId,groupId);

    
 
    //watching movie and wait Movie Techinician to wake them up
    sMT_CR_Stop->P();
    lStopMovie->Acquire();
    cMT_CR_Stop->Signal(lStopMovie);
    lStopMovie->Release();
 

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
        	 printf("DEBUG_O:Customer [%d] in Group [%d] is going to the bathroom.\n",customerId,groupId);
             //goBathroom(); 
             printf("DEBUG_O:Customer [%d] in Group [%d] is leaving the bathroom.\n",customerId,groupId);
             printf("DEBUG_O:Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId); 
    	}else{
        	 printf("DEBUG_O:Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId); 
    	}

printf("DEBUG_O:CCustomer [%d] in Group [%d] , bathroom#[%d]\n",customerId,groupId,groupBathroomSum[groupId]);
       
        
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
         printf("DEBUG_O:Customer [%d] in Group [%d] is going to the bathroom.\n",customerId,groupId); 
         //goBathroom(); 
         sGroupBathroom[groupId]->V();
         printf("DEBUG_O:Customer [%d] in Group [%d] is leaving the bathroom.\n",customerId,groupId);
         sGroupLeaveBathroom[groupId]->V();
         printf("DEBUG_O:Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId);   
    }else{
         printf("DEBUG_O:Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId); 

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
