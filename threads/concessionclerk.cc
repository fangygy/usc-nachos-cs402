// ConcessionClerk
#include "movietheater.h"

Lock foodLineLock("foodLineLock");

ConcessionClerk::ConcessionClerk(int ccId) : Employee(ccId, "ConcessionClerk") {
}
 //concession clerk calculate money
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
 // customer tell cc sum for each food
void ConcessionClerk::setFood(int pos, int sum) {
    food[pos] = sum;
}
//for concession clerk to get food request
int ConcessionClerk::getFood(int pos){

   return food[pos];

}
 //concession clerk sell food and interact with head customer
void ConcessionClerk::sellFood() {
    while(true) {
        lBuyFood->Acquire();
        if (getIsBreak()) {
            //if clerk is on break,wait for manager signal
            printf("%s [%d] is going on break.\n", getEmployeeType(), getId());
            lBuyFood->Release();
            sNoConcessionClerk->P();
            printf("%s [%d] is coming off break.\n", getEmployeeType(), getId());
            lBuyFood->Acquire();
            setIsBreak(false);
        }
        // if have customer, then set busy and signal the customer, else set free
        if (getWaitingSize() > 0) {
            setIsBusy(true);
            printf("ConcessionClerk [%d] has a line length [%d] and is signaling a customer.\n",getId(),getWaitingSize());
            //subWaitingSize();
            condition[0]->Signal(lBuyFood);
        } else {
            printf("ConcessionClerk [%d] has no one in line. I am available for a customer.\n",getId());
            setIsBusy(false);
        }
        lock->Acquire();
        DEBUGINFO('c', "%s [%d] signal customer ", getEmployeeType(), getId());
        // on service
        lBuyFood->Release();
        // wait customer or manager to signal
        condition[1]->Wait(lock);
        DEBUGINFO('c', "%s [%d] wait customer or manager to signal > 0", getEmployeeType(), getId());


        // if break         
        lBuyFood->Acquire();
        if (getIsBreak() && !getIsBusy()) {
            setIsBusy(false);
            lock->Release();
            lBuyFood->Release();
            DEBUGINFO('c', "%s [%d] is break ", getEmployeeType(), getId());
            continue;
        } else {
            setIsBusy(true);
        }
        lBuyFood->Release();
 
        DEBUGINFO('c', "%s [%d] wait customer to order ", getEmployeeType(), getId());
        condition[1]->Signal(lock);
        condition[1]->Wait(lock);
        DEBUGINFO('c', "%s [%d] get money from group [%d]", getEmployeeType(), getId(), getGroupId());
        // tell customers food price 
        calAmount();
         printf("ConcessionClerk [%d] has an order for [%d] popcorn and [%d] soda. The cost is [%.2f].\n",getId(),getFood(FOOD_POPCORN),getFood (FOOD_SODA),getAmount());
        
        condition[1]->Signal(lock);
        // wait for customer pay
        condition[1]->Wait(lock);
        // amount
        lAmount->Acquire();
        concessionClerkAmount[getId()] += getAmount();
        lAmount->Release();
        // get money, handout the food
        condition[1]->Signal(lock);
        printf("ConcessionClerk [%d] has been paid for the order.\n",getId());
        // customer leave, get next customer
        condition[1]->Wait(lock);
        lock->Release();
    }

}
