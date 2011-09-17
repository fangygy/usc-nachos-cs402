// test for part2

#include "system.h"
#include "synch.h"
#include "movietheater.h"

struct CustomerData {
	int customerNumber;
	int groupNumber;
	int groupSize; //Only head customers need this value
};

void test1();

void tc_new(int i) {
    //printf("start tc_new\n");
    //printf("%s: new TicketClerk %d\n", currentThread->getName(), i);
//    tc[i] = new TicketClerk(i);
    tc[i]->sellTickets();
    //printf("%s: TicketClerk %d created\n", currentThread->getName(), i);
}

void cr_tb_new(int arg) {
    //printf("start cr_tb_new\n");
    CustomerData *customerData = (CustomerData *) arg; 
    //printf("%s: new Customer TicketBuyer %d\n", currentThread->getName(), customerData->customerNumber);
    //cr[customerData->customerNumber] = new Customer(customerData->customerNumber, customerData->groupNumber, customerData->groupSize, true);
    cr[customerData->customerNumber]->action();
    //printf("%s: Customer TicketBuyer %d created\n", currentThread->getName(), customerData->customerNumber);
}

void cr_new(int arg) {
    //printf("start cr_new\n");
    CustomerData *customerData = (CustomerData *) arg; 
    //printf("%s: new Customer %d\n", currentThread->getName(), customerData->customerNumber);
    //cr[customerData->customerNumber] = new Customer(customerData->customerNumber, customerData->groupNumber, customerData->groupSize, false);
    cr[customerData->customerNumber]->action();
    //printf("%s: Customer TicketBuyer %d created\n", currentThread->getName(), customerData->customerNumber);
}

void cc_new(int i) {
    //printf("start cc_new\n");
    //printf("%s: new ConcessionClerk %d\n", currentThread->getName(), i);
    //cc[i] = new ConcessionClerk(i);
    cc[i]->sellFood();
    //printf("%s: ConcessionClerk %d created\n", currentThread->getName(), i);
}

void tt_new(int i) {
   // printf("start tt_new\n");
    //printf("%s: new TicketTaker %d\n", currentThread->getName(), i);
    //tt[i] = new TicketTaker(i);
    tt[i]->checkTickets();
   // printf("%s: TicketTaker %d created\n", currentThread->getName(), i);
}

void mt_new(int i) {
   // printf("start mt_new\n");
   // printf("%s: new MovieTechnician %d\n", currentThread->getName(), i);
   // mt[i] = new MovieTechnician(i);
  //  printf("%s: MovieTechnician %d created\n", currentThread->getName(), i);
}

void mr_new(int i) {
    //printf("start mr_new\n");
    //printf("%s: new Manager %d\n", currentThread->getName(), i);
    //mr[i] = new Manager(i);
    test1();
}


void test1() {
    char *name;
    Thread *t;

    // create TicketClerk
    for (int i = 0;i < MAX_TC; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_TicketClerk_%d", i); 
        // ? in thread or hear ?
//        tc[i] = new TicketClerk(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)tc_new, i);
    }
    printf("Number of TicketClerks = [%d]\n",MAX_TC);
    // create Customer
    int group = rand()%MAX_GROUP + 1;
    int nextCustomerNumber = 0; 
    for (int i = 0;i < group; ++i) {
        int groupSize = rand()%MAX_GROUPSIZE+1;
        for (int j = 0;j < groupSize - 1; ++j) {
            name = new char[MAX_VAR];
            sprintf(name, "Thread_Customer_%d", nextCustomerNumber);
            CustomerData *customerData = new CustomerData;
            customerData->customerNumber = nextCustomerNumber++;
            customerData->groupNumber = i;
            customerData->groupSize = groupSize;
            // ? in thread or here
            cr[customerData->customerNumber] = new Customer(customerData->customerNumber, customerData->groupNumber, customerData->groupSize, false);
            t = new Thread(name);
            t->Fork((VoidFunctionPtr)cr_new, int(customerData));
        }
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Head_Customer_%d", nextCustomerNumber);
        CustomerData *customerData = new CustomerData;
        customerData->customerNumber = nextCustomerNumber++;
        customerData->groupNumber = i;
        customerData->groupSize = groupSize;
        // ? in thread or here
        cr[customerData->customerNumber] = new Customer(customerData->customerNumber, customerData->groupNumber, customerData->groupSize, true);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)cr_tb_new, int(customerData));
    }
    printf("Number of Customers = [%d]\n",nextCustomerNumber);
    printf("Number of Groups = [%d]\n",group);
    // create ConcessionClerk 
    for (int i = 0;i < MAX_CC; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_ConcessionClerk_%d", i);
        // ? in thread or here
//        cc[i] = new ConcessionClerk(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)cc_new, i);
    }
    printf("Number of ConcessionClerks = [%d]\n",MAX_CC);
    // create TicketTaker 
    for (int i = 0;i < MAX_TT; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_TicketTaker_%d", i);
        // ? in thread or here
//        tt[i] = new TicketTaker(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)tt_new, i);
    }
    printf("Number of TicketTakers = [%d]\n",MAX_TT);

    // create MovieTechnician 
    for (int i = 0;i < MAX_MT; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_MovieTechnician_%d", i);
        // ? in thread or here
//        mt[i] = new MovieTechnician(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mt_new, i);
    }
     
//    mr[0]->work();
}
void Problem2() {
    init();
    char name[MAX_VAR];
    Thread * t; 
    // create Manager 
    for (int i = 0;i < MAX_MR; ++i) {
        sprintf(name, "Thread_Manager_%d", i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mr_new, i);
    }

}
