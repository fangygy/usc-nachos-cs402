// Customer
#include "movietheater.h"

Customer::Customer(int cId, int gId, int gSize, bool ticketBuyer) {
    customerId = cId;
    groupId = gId;
    groupSize = gSize;
    isTicketBuyer = ticketBuyer;
    // how to find groupmate
    //Customer * customers;   //Only head customers need this value
}
Customer::~Customer() {}

void Customer::action() {
    if (getIsTicketBuyer()) {
        // ticket buyer
        buyTickets();
        buyFood();
        checkTickets();
    } else {
        // regular customer
        
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
        employee[lineIndex]->addWaitingSize();
    }
    return lineIndex; 
}
void Customer::buyTickets() {

    int lineIndex = -1;
    printf("Customer %d try to get in line\n",customerId);
    printTCStatus();

    while (lineIndex == -1) {
        lineIndex = getInLine(lBuyTickets, (Employee**)tc, MAX_TC);        
    }
    // get in which line
    printf("customer %d : found the shortest line %d\n", customerId, lineIndex);
    TicketClerk *clerk = tc[lineIndex];
    // if busy wait for ticketClerk
    if (clerk->getIsBusy()) {
        clerk->condition[0]->Wait(lBuyTickets);
    }
    printf("customer %d : is interacting with clerk %d\n", customerId, lineIndex);
    printTCStatus();
    printf("============================\n");
    // interact with TicketClerk
    clerk->lock->Acquire();
    if (!clerk->getIsBreak()) {
        lBuyTickets->Release(); 
        // tell clerk tickets sum
        clerk->setTicketSum(groupSize);
        clerk->condition[1]->Signal(clerk->lock);

        // wait for money amount
        clerk->condition[1]->Wait(clerk->lock);
        double amount = clerk->getAmount();
        clerk->setPayment(amount);
        // TODO: print out amount and give money
        printf("print out amount and give money\n");
        clerk->condition[1]->Signal(clerk->lock);

        // wait to get tickets back
        clerk->condition[1]->Wait(clerk->lock);
        // finish and leave
        clerk->condition[1]->Signal(clerk->lock);
    } else {
        clerk->subWaitingSize();
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

    ConcessionClerk *clerk = cc[lineIndex];
    clerk->condition[0]->Wait(lBuyFood);
    lBuyFood->Release();
    // interact with ConcessionClerk 
    clerk->lock->Acquire();
    if (!clerk->getIsBreak()) {
        // TODO: tell clerk popcorn sum
        int popcorn = countFood("popcorn");
        clerk->condition[1]->Signal(clerk->lock);

        // wait for acknowledgement 
        clerk->condition[1]->Wait(clerk->lock);

        // TODO: tell clerk soda sum
        int soda = countFood("soda");
        clerk->condition[1]->Signal(clerk->lock);

        // wait for amount 
        clerk->condition[1]->Wait(clerk->lock);
        // TODO: give money
        double amount = clerk->getAmount();
        clerk->setPayment(amount);
        clerk->condition[1]->Signal(clerk->lock);
    }
    clerk->lock->Release();

}

int Customer::countFood(char * food) {
    if (getIsTicketBuyer()) {
        // TODO
        return 1;
    }
    return 0;
}

bool Customer::askForFood(char * food) {
    return true;
}

bool Customer::answerForFood(char * food) {
    return true;
}

void Customer::checkTickets() {
    int lineIndex = -1;
    while (lineIndex == -1) {
        lineIndex = getInLine(lCheckTickets, (Employee**)tt, MAX_TT);
    }

    TicketTaker *clerk = tt[lineIndex];
    clerk->condition[0]->Wait(lCheckTickets);
    lCheckTickets->Release();	
    // interact with TicketTaker 
    clerk->lock->Acquire();
    if (!clerk->getIsBreak()) {
        clerk->condition[1]->Signal(clerk->lock);
        // TODO:  

        clerk->condition[1]->Wait(clerk->lock);
        // TODO: give money
        clerk->condition[1]->Signal(clerk->lock);
    }
    clerk->lock->Release();
}

void Customer::goBathroom() {
    ;
}
void Customer::leaveTheater() {
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
