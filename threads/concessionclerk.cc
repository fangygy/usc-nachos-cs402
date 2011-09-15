// ConcessionClerk
#include "movietheater.h"

Lock foodLineLock("foodLineLock");

ConcessionClerk::ConcessionClerk(int ccId) : Employee(ccId, "ConcessionClerk") {
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

void ConcessionClerk::sellFood() {
    while(true) {
        if (getIsBreak()) {
            continue;
        }
        lBuyFood->Acquire();
        // if have customer, then set busy and signal the customer, else set free
        if (getWaitingSize() > 0) {
            setIsBusy(true);
            subWaitingSize();
            condition[0]->Signal(lBuyFood);
        } else {
            setIsBusy(false);
        }
        // on service
        lock->Acquire();
        lBuyFood->Release();
        // wait customer or manager to signal
        condition[1]->Wait(lock);
        // ? break ?
        //if (getIsBreak()) {
        //    setIsBusy(false);
        //    lock->Release();
        //    continue;
        //}

        // TODO: tell customers amount
        condition[1]->Signal(lock);
        condition[1]->Wait(lock);
        // get money, handout the tickets
        condition[1]->Signal(lock);
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
