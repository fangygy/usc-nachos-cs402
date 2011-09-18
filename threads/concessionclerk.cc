// ConcessionClerk
#include "movietheater.h"

Lock foodLineLock("foodLineLock");

ConcessionClerk::ConcessionClerk(int ccId) : Employee(ccId, "ConcessionClerk") {
}
void ConcessionClerk::calAmount() {
    amount = food[FOOD_POPCORN]*PRICE_POPCORN +food[FOOD_SODA]*PRICE_SODA;
}
double ConcessionClerk::getAmount() {
    return amount;
}
void ConcessionClerk::setAmount(double a) {
    amount = a;
}

void ConcessionClerk::setPayment(double p) {
    payment = p;
}
double ConcessionClerk::getPayment() {
    return payment;
}
void ConcessionClerk::setFood(int pos, int sum) {
    food[pos] = sum;
}
int ConcessionClerk::getFood(int pos){

   return food[pos];

}



void ConcessionClerk::sellFood() {
    while(true) {
        lBuyFood->Acquire();
        if (getIsBreak()) {
            // wait for manager signal
            printf("%s [%d] is going on break.\n", getEmployeeType(), getId());
            lBuyFood->Release();
            sNoTicketTaker->P();
            printf("%s [%d] is coming off break.\n", getEmployeeType(), getId());
            lBuyFood->Acquire();
            setIsBreak(false);

//            printf("ConcessionClerk [%d] is going on break.\n",getId());
//            continue;
        }
        // if have customer, then set busy and signal the customer, else set free
        if (getWaitingSize() > 0) {
            setIsBusy(true);
            printf("ConcessionClerk [%d] has a line length [%d] and is signaling a customer.\n",getId(),getWaitingSize());
            subWaitingSize();
            condition[0]->Signal(lBuyFood);
        } else {
            printf("ConcessionClerk [%d] has no one in line. I am available for a customer.\n",getId());
            setIsBusy(false);
        }
        lock->Acquire();
        // on service
        lBuyFood->Release();
        // wait customer or manager to signal
        condition[1]->Wait(lock);
        // if break 
        if (getIsBreak()) {
            setIsBusy(false);
            lock->Release();
            continue;
        } else {
            setIsBusy(true);
        }

       



        condition[1]->Signal(lock);
        condition[1]->Wait(lock);
        // tell customers food price 
        calAmount();
         printf("ConcessionClerk [%d] has an order for [%d] popcorn and [%d] soda. The cost is [%.2f].\n",getId(),getFood(FOOD_POPCORN),getFood (FOOD_SODA),getAmount());
        
        condition[1]->Signal(lock);
        // wait for customer go pay
        condition[1]->Wait(lock);
        // get money, handout the food
        condition[1]->Signal(lock);
        printf("ConcessionClerk [%d] has been paid for the order.\n",getId());
        // customer leave, get next customer
        condition[1]->Wait(lock);
        lock->Release();
    }

}
void ConcessionClerk::printStatus(){

    //Print Clerk Status

    printf("Concession Clerk Status:");
    for (int i = 0;i < MAX_CC; ++i) {
        ConcessionClerk *clerk = cc[i];
        if(clerk!=NULL){ // if clerk have not been create, don't print    
            printf("%d,",clerk->getIsBusy());
        }else{
            printf("NA,"); 
        }    
    }
    printf("\n");


    //Print the number of waiting customer in each clerk's line

    printf("Concession counter Waiting Status (#):");

    for (int i = 0;i < MAX_CC; ++i) {
        ConcessionClerk *clerk = cc[i];
        if(clerk!=NULL){ // if clerk have not been create, don't print
            printf("%d,",clerk->getWaitingSize());
        }else{
            printf("NA,"); 
        }
    }
    printf("\n");

}
