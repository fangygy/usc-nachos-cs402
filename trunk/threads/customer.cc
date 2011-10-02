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

void Customer::action() {
    
    //Is head customer?
    if (getIsTicketBuyer()) {
        // ticket buyer
        DEBUG('z', "\tGroup [%d] start buy tickets.\n", groupId);
        //Head customer buy ticket for group member
        buyTickets();
        DEBUG('z', "\tGroup [%d] finish buy tickets.\n", groupId);
        //Head customer has buy the tickets and ask members to proceed to concession
        proceed(groupTicket);
        DEBUGINFO('c', "Customer [%d] in group [%d] finish waitGroup after finish buy tickets", customerId, groupId);

        //Head customer ask group member if they want food. If no food requirement,go to ticket taker and check ticket
        if (countFood()) {
            DEBUGINFO('c', "Customer [%d] in group [%d] start buy food", customerId, groupId);
            //Head customer buy food for group member.
            buyFood();
        }
        DEBUGINFO('c', "Customer [%d] in group [%d] finish buy food", customerId, groupId);
        //Head customer has buy food and ask members to proceed to check ticket
        proceed(groupFood);
        DEBUGINFO('c', "Customer [%d] in group [%d] start check tickets.", customerId, groupId);
        DEBUG('z', "\tGroup [%d] start \n", groupId);
        //Head customer go to check ticket for group member
        checkTickets();
        DEBUGINFO('c', "Customer [%d] in group [%d] finish check Ticket", customerId, groupId);
        DEBUG('z', "\tGroup [%d] finish check tickets.\n", groupId);
        proceed(groupSeat);
        //Head customer have ticket checked for group member and ask member to enter theater room
        DEBUGINFO('c', "Customer [%d] in group [%d] start take seat", customerId, groupId);
        DEBUG('z', "\tGroup [%d] start take seat.\n", groupId);
        //Head customer find seat for group member
        arrangeSeats();
        DEBUGINFO('c', "Customer [%d] in group [%d] finish take seat", customerId, groupId);
        DEBUG('z', "\tGroup [%d] finish take seat.\n", groupId);
        //Head customer watch movie
        watchMovie();
        DEBUGINFO('c', "Customer [%d] in group [%d] finish watch Movie", customerId, groupId);
        //Head customer ask group member to leave theater room
        proceed(groupLeaveRoom);
        prtLeaveRoomMsg();
        DEBUGINFO('c', "Customer [%d] in group [%d] leave theater room", customerId, groupId);
        //Head customer ask group member if they want to go to bathroom and wait for them to leave theater
        checkBathroom();
        DEBUGINFO('c', "Customer [%d] in group [%d] leave bath room", customerId, groupId);
        //Head customer ask group member to leave theater
        proceed(groupLeaveTheater);
         //Head customer and group member have left theater
        leaveTheater(); 
        DEBUGINFO('c', "Customer [%d] in group [%d] leave theater", customerId, groupId);
    } else {
        // regular customer
        DEBUGINFO('c', "Customer [%d] in group [%d] waitTickets", customerId, groupId);
        //regular customer wait forhead customer to buy tickets
        waitTickets();
        DEBUGINFO('c', "Customer [%d] in group [%d] waitFood", customerId, groupId);
       //regular customer wait forhead customer to buy Food    
        waitFood();  
        DEBUGINFO('c', "Customer [%d] in group [%d] waitCheck", customerId, groupId);
       //regular customer wait for head customer to check ticket    
        waitCheck();   
        DEBUGINFO('c', "Customer [%d] in group [%d] waitSeats", customerId, groupId); 
       //regular customer wait for head customer to arrange food
        waitSeats();
        DEBUGINFO('c', "Customer [%d] in group [%d] watchMovie", customerId, groupId);
       //regular customer watch movie
        watchMovie();
        DEBUGINFO('c', "Customer [%d] in group [%d] waitLeaveRoom", customerId, groupId);
       //regular customer wait for head customer to ask them to leave theater room together
        waitLeaveRoom();
        prtLeaveRoomMsg();
        DEBUGINFO('c', "Customer [%d] in group [%d] waitBathroom", customerId, groupId);
       //regular customer wait for head customer to ask if they need to go bathroom
        waitBathroom();
        DEBUGINFO('c', "Customer [%d] in group [%d] waitLeaveTheater", customerId, groupId);
       //regular customer wait for head customer to ask them to leave theater together
        waitLeaveTheater();
        //regular customer leave theater
        leaveTheater(); 
        DEBUGINFO('c', "Customer [%d] in group [%d] leave theater", customerId, groupId);
    }
}
bool Customer::getIsTicketBuyer() {
    //check is head customer
    return isTicketBuyer;
}


//Head customer try to get in shortest line
int Customer::getInLine(Lock *lock, Condition *cNoClerk, int count, bool &noClerk, Employee** employee ) {


    int i, j;
    int lineIndex = -1;
    int lineLen = -1;

    //get in line lock to make sure no one can get in line at the same time
    lock->Acquire();
    DEBUG('z', "\tGroup [%d] start find line\n", groupId);    

    //check every clerk
    for (i = 0;i < count; ++i) {
        // if the clerk is not busy or not on break and no one wait in line, go to the line directly 
        if (!employee[i]->getIsBusy() && !employee[i]->getIsBreak() && employee[i]->getWaitingSize() == 0) {
            lineIndex = i;
            break;
        }
        
        // if clerk is not on break, find a shortest line
        if (!employee[i]->getIsBreak() && (lineIndex == -1 || employee[i]->getWaitingSize() < lineLen)) {
            lineLen = employee[i]->getWaitingSize();
            lineIndex = i;
        }
    }
    DEBUG('z', "\tGroup [%d] finish find line\n", groupId);    

    if (lineIndex == -1) {
         // if all clerk are on break
        // need to wait until the manager signal them
        DEBUGINFO('c', "Customer [%d] in Group [%d] find noClerk %s, should wake up someone", customerId, groupId, employee[0]->getEmployeeType());
        noClerk = true;
        printf("Customer [%d] in Group [%d] is in the lobby.\n", customerId, groupId);
        cNoClerk->Wait(lock);
        printf("Customer [%d] in Group [%d] is leaving the lobby.\n",  customerId, groupId);
        return lineIndex;

    } else {
        //if head customer find the shortest line, waiting in line customer number add 1        
        DEBUG('z', "\tbusy%d\n", employee[lineIndex]->getIsBusy()?1:0);
        employee[lineIndex]->addWaitingSize();
    }
    DEBUG('z', "\t%dline%d\n", lineIndex, employee[lineIndex]->getWaitingSize());
    DEBUGINFO('c', "\tCustomer [%d] in Group [%d] get in %s line [%d], length:%d, busy:%d\n", customerId, groupId, employee[lineIndex]->getEmployeeType(), lineIndex, employee[lineIndex]->getWaitingSize(), employee[lineIndex]->getIsBusy()?1:0);


    return lineIndex; 
}
//Head customer buy ticket for group member
void Customer::buyTickets() {

    int lineIndex = -1;

    //lBuyTickets->Acquire();
    //Head customer try to get in shortest line
    while (lineIndex == -1) {
        lineIndex = getInLine(lBuyTickets, cNoTicketClerk, tcNumber, noTicketClerk, (Employee**)tc);        
    }
    
    // get in which line
    printf("Customer [%d] in Group [%d] is getting in TicketClerk line [%d]\n",customerId,groupId,lineIndex);

 
    TicketClerk *clerk = tc[lineIndex];
   // if the clerk is busy serving other customer,wait for the clerk to wake them up 
    DEBUGINFO('c', "Customer [%d] in Group [%d] get in %s line [%d], length:%d, busy:%d\n", customerId, groupId, clerk->getEmployeeType(), lineIndex, clerk->getWaitingSize(), clerk->getIsBusy()?1:0);
    if (clerk->getIsBusy()) {
        clerk->condition[0]->Wait(lBuyTickets);
    }
    printf("Customer [%d] in Group [%d] is walking up to TicketClerk [%d] to buy [%d] tickets\n",customerId,groupId,lineIndex,groupSize);
    
    DEBUGINFO('c', "Customer [%d] in group [%d] acquire lock [%s], owner %s", customerId, groupId, clerk->lock->getName(), clerk->lock->getOwnerThread()!=NULL?clerk->lock->getOwnerThread()->getName():"NULL");
    
    // buy ticket :interact with TicketClerk
    clerk->lock->Acquire();

    //waiting line number minus 1
    clerk->subWaitingSize();

    if (!clerk->getIsBreak()) {
        clerk->setIsBusy(true);
        lBuyTickets->Release(); 
         // tell clerk tickets sum
        clerk->setGroupId(groupId);
        clerk->setTicketSum(groupSize);
        clerk->condition[1]->Signal(clerk->lock);
        DEBUGINFO('c', "1 Customer [%d] in Group [%d] is walking up to TicketClerk [%d] to buy [%d] tickets\n",customerId,groupId,lineIndex,groupSize);
        // wait for clerk to tell how much money
        clerk->condition[1]->Wait(clerk->lock);
        // ask ticket clerk for total price
        double amount = clerk->getAmount();
        // pay money 
        clerk->setPayment(amount);
        printf("Customer [%d] in Group [%d] in TicketClerk line [%d] is paying [%.2f] for tickets\n",customerId,groupId,lineIndex,amount);
        DEBUGINFO('c', "2 Customer [%d] in Group [%d] in TicketClerk line [%d] is paying [%.2f] for tickets\n",customerId,groupId,lineIndex,amount);
        //to give the clerk money
        clerk->condition[1]->Signal(clerk->lock);
        // wait to get tickets back
        clerk->condition[1]->Wait(clerk->lock);
        // finish and leave
        clerk->condition[1]->Signal(clerk->lock);
        DEBUGINFO('c', "3 Customer [%d] in Group [%d] is leaving TicketClerk[%d]\n",customerId,groupId,lineIndex);
        printf("Customer [%d] in Group [%d] is leaving TicketClerk[%d]\n",customerId,groupId,lineIndex);
    } else {
        //if the clerk is on break,head customer need to start over and get another line
        printf("Customer [%d] in Group [%d] sees TicketClerk [%d] is on break.\n",customerId,groupId,lineIndex);
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
    //Head customer try to get in shortest line
    while (lineIndex == -1) {
        lineIndex = getInLine(lBuyFood, cNoConcessionClerk, ccNumber, noConcessionClerk, (Employee**)cc);
    }
    DEBUGINFO('c', "Customer [%d] in group [%d] get in ConcessionClerk [%d]", customerId, groupId, lineIndex);
    printf("Customer [%d] in Group [%d] is getting in ConcessionClerk line [%d]\n",customerId,groupId,lineIndex);
   
    ConcessionClerk *clerk = cc[lineIndex];

   // if the clerk is busy serving other customer,wait for the clerk to wake them up 
    if (clerk->getIsBusy()) {
        clerk->condition[0]->Wait(lBuyFood);
    DEBUGINFO('c', "Customer [%d] in group [%d] is wake up to goto ConcessionClerk [%d]", customerId, groupId, lineIndex);
    }
    DEBUGINFO('c', "Customer [%d] in group [%d] is walking up to ConcessionClerk [%d]", customerId, groupId, lineIndex);
    printf("Customer [%d] in Group [%d] is walking up to ConcessionClerk[%d] to buy [%d] popcorn and [%d] soda.\n",customerId,groupId,lineIndex,groupFoodSum[groupId][0],groupFoodSum[groupId][1]);

    DEBUGINFO('c', "Customer [%d] in group [%d] acquire lock [%s], owner %s", customerId, groupId, clerk->lock->getName(), clerk->lock->getOwnerThread()!=NULL?clerk->lock->getOwnerThread()->getName():"NULL");
    //buyfood: interact with ConcessionClerk 
    clerk->lock->Acquire();
    DEBUGINFO('c', "Customer [%d] in group [%d] acquired lock [%d], owner %s", customerId, groupId, clerk->lock->getName(), clerk->lock->getOwnerThread()!=NULL?clerk->lock->getOwnerThread()->getName():"NULL");

    //waiting line number minus 1    
    clerk->subWaitingSize();

    if (!clerk->getIsBreak()) {
        clerk->setIsBusy(true);
        lBuyFood->Release();

        // tell clerk popcorn sum and soda sum
        clerk->setGroupId(groupId);
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
        //if the clerk is on break,head customer need to start over and get another line
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
//Head customer check with each group member if they want food
bool Customer::countFood() {

    if (getIsTicketBuyer()) {
        lGroup[groupId]->Acquire();
        // if head customer self want food
        for (int i = 0;i < 2; ++i) {
            groupFoodSum[groupId][i] += answerForFood(i);
        }
        // ask group mate if they want food
        cGroupFood[groupId]->Broadcast(lGroup[groupId]);
        // set monitor variable true
        groupAskForFood[groupId] = true;
        lGroup[groupId]->Release();

        // wait for all the customer answer for food
        for (int i = 1;i < groupSize; ++i) {
            sGroupFood[groupId]->P();
        }
        printf("Customer [%d] in Group [%d] has [%d] popcorn and [%d] soda request from a group member\n",customerId,groupId,groupFoodSum[groupId][0],groupFoodSum[groupId][1]);

        //if group member don't need any food
        if (!groupFoodSum[groupId][0] && !groupFoodSum[groupId][1]) {
            groupFood[groupId] = true;
            return false;
        }
        return true;
    }
    return false;
}
// rand for food request
int Customer::answerForFood(int food) {
    int choice = rand()%4;
    return (rand()%4) > 0;
}
//Head customer go to ticket taker and check ticket
void Customer::checkTickets() {

    int lineIndex = -1;
    //Head customer try to get in shortest line
    while (lineIndex == -1) {
        lineIndex = getInLine(lCheckTickets, cNoTicketTaker, ttNumber, noTicketTaker, (Employee**)tt);
    }
    
    printf("Customer [%d] in Group [%d] is getting in TicketTaker line [%d]\n",customerId,groupId,lineIndex);
    TicketTaker *clerk = tt[lineIndex];
    DEBUGINFO('c', "Customer [%d] in group [%d] get in %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);

  // if the clerk is busy serving other customer,wait for the clerk to wake them up 
    if (clerk->getIsBusy()) {
        DEBUGINFO('c', "Customer [%d] in group [%d] wait for TicketTaker [%d]", customerId, groupId, lineIndex);
        clerk->condition[0]->Wait(lCheckTickets);
        
    }
    DEBUGINFO('c', "Customer [%d] in group [%d] acquire lTicketTaken with %s [%d], lTicketTaken's owner is: %s", customerId, groupId, clerk->getEmployeeType(), lineIndex, lTicketTaken->getOwnerThread()==NULL?"NULL":lTicketTaken->getOwnerThread()->getName());
   //ticket taken : interact with ticket taker 
    lTicketTaken->Acquire();
   //waiting line number minus 1    
    clerk->subWaitingSize();

    if (stopTicketTaken) {
       // if the theater room is full, the ticket taker will stop take ticket
        DEBUGINFO('c', "Customer [%d] in group [%d] sees in TicketTaker line [%d] no longer taking tickets", customerId, groupId, lineIndex);
        //ask all group to goto lobby
        printf("Customer [%d] in Group [%d] sees TicketTaker [%d] is no longer taking tickets. Going to the lobby.\n", customerId, groupId, lineIndex);
        printf("Customer [%d] in Group [%d] is in the lobby.\n", customerId, groupId);
        lCheckTickets->Release();
        lTicketTaken->Release();
        clerk->lock->Acquire();
        //if restart take ticket, they will wake customer up and the customer will get in line again
        clerk->condition[1]->Signal(clerk->lock);
        clerk->lock->Release();
        lTicketTaken->Acquire();
        DEBUGINFO('c', "Customer [%d] in group [%d] wait TicketTaken", customerId, groupId);
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

    // ticker take: interact with TicketTaker 
    clerk->lock->Acquire();
    if (!clerk->getIsBreak() ) {
        DEBUGINFO('c', "Customer [%d] in group [%d] get lock with %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        clerk->setIsBusy(true);
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        lCheckTickets->Release();
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        clerk->setGroupId(groupId);
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        // give ticket taker ticket
        clerk->setTicketSum(ticketReceipt[groupId]);
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        clerk->condition[1]->Signal(clerk->lock);
        DEBUGINFO('c', "Customer [%d] in group [%d] wait %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        // wait for ticket taker to check ticket
        clerk->condition[1]->Wait(clerk->lock);
        DEBUGINFO('c', "Customer [%d] in group [%d] get lock with %s [%d]", customerId, groupId, clerk->getEmployeeType(), lineIndex);
        
        // if ticket taker foound the seats in the theater room are just not enough for the group, ticket taker stop take ticket
//        lTicketTaken->Acquire();
        if (stopTicketTakenArr[lineIndex]) {
            // ask all group to go back lobby and wait for recheck the tickets
            clerk->lock->Release();
            printf("Customer [%d] in Group [%d] sees TicketTaker [%d] is no longer taking tickets. Going to the lobby.", customerId, groupId, lineIndex);
            printf("Customer [%d] in Group [%d] is in the lobby.\n", customerId, groupId);
            DEBUGINFO('c', "Customer [%d] in group [%d] wait to retake ticket", customerId, groupId, lineIndex);
            lTicketTaken->Acquire();
            if (stopTicketTaken) {
                cTicketTaken->Wait(lTicketTaken);
            }
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
         //if the clerk is on break,head customer need to start over and get another line
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

// rand for going to bathroom
bool Customer::answerForBathroom() {
    
    int chance = rand()%100;
    if(chance<=25){
        return true;
    }
    return false;
}

//customer go to bathroom
void Customer::goBathroom() {
        
      int i=0;
     for (i = 0; i < 100; i++) {
            currentThread->Yield();
     }



}
//customer leave theater
void Customer::leaveTheater() {
    // if leave, --customerLeft
    lCustomerLeft->Acquire(); 
    --customerLeft;
    lCustomerLeft->Release();
    printf("Customer [%d] in Group [%d] has left the movie theater\n",customerId,groupId); 
}
// Head customer get seat for the group
   
void Customer::arrangeSeats() {

    

    // lock the seats and make sure no other head customer is going to arrange seats 
    lFindSeats->Acquire();
int i=0;    
    DEBUGINFO('c', "SeatAvailable group size[%d]\n",groupSize);  

    //get seat number 
    getSeats(groupSize);
    seatPos=SeatLocation[0];	
    printf("Customer [%d] in Group [%d] has found the following seat: row [%d] and seat [%d]\n",customerId,groupId,(seatPos/MAX_ROW),seatPos%MAX_COL);  
  
    
    lGroup[groupId]->Acquire();
    DEBUGINFO('c', "Customer [%d] in Group [%d] arrange seat %d", customerId,groupId,SeatLocation[0]);
    seatState[SeatLocation[0]]=true;
 
    // tell other group member the seat number 
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
//Head customer find available seats
int Customer::getSeats(int Size){
    
    int i=0,j=0;
    int consecutive=0,RequestSeat=0;
    int SeatAvailable[MAX_ROW]={0};

    bool bFind=false;
    
    //assumption:customer always choose seat from the beginning of a row
     //count available seats
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
            SeatAvailable[i]=SeatAvailable[i]-Size;
            break;
        }
    }
    //if there is no consecutive seats, choose available seats
    if(bFind==false){
        for(i=0;i<MAX_ROW;i++){ 
                while(SeatAvailable[i]!=0&& RequestSeat<Size){
                        SeatLocation[RequestSeat]=(MAX_COL-SeatAvailable[i])+i*MAX_COL; 
                        DEBUGINFO('c', "SeatAvailable RequestSeat[%d][%d][%d]",i, SeatAvailable[i],RequestSeat); 
                        RequestSeat++;   
                        SeatAvailable[i]--;     
                  }
        }
            
    }

    return 1;
}
// regular customer wait for head customer to have ticket checked
void Customer::waitTickets() {

    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
 
    //wait for head customer to tell them the tickets checked
    // if head customer had broadcast, do not wait
    printf("Customer [%d] of group [%d] is waiting for the HeadCustomer.\n",customerId,groupId);
    if (!groupTicket[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    //tickets check and go to the next step
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
    //random the food request
    for (int i = 0;i < 2; ++i) {
        groupFoodSum[groupId][i] += answerForFood(i);
    }

    // tell head customer that they have chosen food
    sGroupFood[groupId]->V();
    // wait for proceed
    sGroup[groupId]->V();

    //wait for head customer to get the food
    // if ticketbuyer had broadcast, do not wait
    printf("Customer [%d] of group [%d] is waiting for the HeadCustomer.\n",customerId,groupId);
    if (!groupFood[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    // after get food, go to the next step
    printf("Customer [%d] of group [%d] has been told by the HeadCustomer to proceed.\n", customerId, groupId);
    lGroup[groupId]->Release();
}
//regular customer wait head customer to have the tickets check
void Customer::waitCheck() {

    DEBUGINFO('c', "Customer [%d] in group [%d] wait check a", customerId, groupId);
    
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
    DEBUGINFO('c', "Customer [%d] in group [%d] wait check b", customerId, groupId);

    //wait for the head customer to have ticket checked
    printf("Customer [%d] of group [%d] is waiting for the HeadCustomer.\n",customerId,groupId);
    if (!groupSeat[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    //if ticket checked, go to the next step
    DEBUGINFO('c', "Customer [%d] in group [%d] has been told by the HeadCustomer to proceed.", customerId, groupId);
    printf("Customer [%d] of group [%d] has been told by the HeadCustomer to proceed.\n", customerId, groupId);
   
    lGroup[groupId]->Release();

}
//regular customer wait head customer to tell the seat number
void Customer::waitSeats() {
 
    //wait for head to get seats
    sWaitSeat[groupId]->P();
    lGroup[groupId]->Acquire();
    // got the seat number from head customer and then info head customer
    printf("Customer [%d] in Group [%d] has found the following seat: row [%d] and seat [%d]\n",customerId,groupId,(seatPos/MAX_ROW),(seatPos%MAX_COL));   
    cGroup[groupId]->Signal(lGroup[groupId]);
    lGroup[groupId]->Release();
       
        
}

//customers wait fot movie to start
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




 //DEBUGINFO('c', "%s P() %s value %d", currentThread->getName(), getName(), value);

    //watching movie and wait Movie Techinician to wake them up
    sMT_CR_Stop->P();
    DEBUGINFO('c', "Customer [%d] in group [%d] acquire lStopMovie, lStopMovie's owner : %s", customerId,groupId, lStopMovie->getOwnerThread() == NULL? "NULL": lStopMovie->getOwnerThread()->getName());
    lStopMovie->Acquire();
    DEBUGINFO('c', "Customer [%d] in group [%d] end acquire lStopMovie, lStopMovie's owner : %s", customerId,groupId, lStopMovie->getOwnerThread() == NULL? "NULL": lStopMovie->getOwnerThread()->getName());
    cMT_CR_Stop->Signal(lStopMovie);
    lStopMovie->Release();
    DEBUGINFO('c', "Customer [%d] in group [%d] is watching movie and wait up by Movie Techinician.", customerId, groupId);

}
// regular customer wait for head customer to ask them leave theater room together
void Customer::waitLeaveRoom(){

    //regroup 
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
    if (!groupLeaveRoom[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    lGroup[groupId]->Release();

}

//Head customer ask group member if they want to go to bathroom and wait for them to leave theater
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

   // check if head customer want to go to bathroom 
    if(answerForBathroom()){
        printf("Customer [%d] in Group [%d] is going to the bathroom.\n",customerId,groupId);
        //goBathroom(); 
        printf("Customer [%d] in Group [%d] is leaving the bathroom.\n",customerId,groupId);
        printf("Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId); 
    }else{
      	 printf("Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId); 
    }
       
    // wait for regular customer come back from bathroom    
    for (int i = 1;i < groupBathroomSum[groupId]; ++i) {
        sGroupLeaveBathroom[groupId]->P();
    }

}
//regular customer wait for head customer to ask if they need to go bathroom
void Customer::waitBathroom(){

    lGroup[groupId]->Acquire();
    // if head customer had broadcast, don't wait
    if (!groupAskForBathroom[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    
    lGroup[groupId]->Release();

  
    if(answerForBathroom()){
        //if want go bathroom,the number of customer going bathroom plus 1
        lBathroomNum[groupId]->Acquire();
         groupBathroomSum[groupId]++;
        lBathroomNum[groupId]->Release(); 
         printf("Customer [%d] in Group [%d] is going to the bathroom.\n",customerId,groupId); 
         //goBathroom();  
         sGroupBathroom[groupId]->V();
         printf("Customer [%d] in Group [%d] is leaving the bathroom.\n",customerId,groupId);
         //tell head customer back from bathroom        
         sGroupLeaveBathroom[groupId]->V();
         printf("Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId);   
    }else{
         //the customer don't want go bathroom
         printf("Customer [%d] in Group [%d] is in the lobby.\n",customerId,groupId); 
         sGroupBathroom[groupId]->V();
    }

    
}
//show that customer is going out ot theater room
void Customer::prtLeaveRoomMsg(){

    printf("Customer [%d] in group [%d] is getting out of a theater room seat.\n",customerId,groupId);
}
//regular customer wait for head customer to ask them to leave theater together
void Customer::waitLeaveTheater(){

    //regroup 
    sGroup[groupId]->V();
    lGroup[groupId]->Acquire();
    if (!groupLeaveTheater[groupId]) {
        cGroup[groupId]->Wait(lGroup[groupId]);
    }
    lGroup[groupId]->Release();

}
//head customer ask member to go to the next step
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
//get customer grooup id
int Customer::getGroupId(){

    return   groupId;
}
//get customer id
int Customer::getId(){

    return   customerId;

}
