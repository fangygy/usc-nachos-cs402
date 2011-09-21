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
bool test_case();
void TestCase_P2_1();
void TestCase_P2_2();
void TestCase_P2_3();
void TestCase_P2_4();
void TestCase_P2_5();
void TestCase_P2_6();

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
    mt[i]->WaitManager();
}

void mr_new(int i) {
    //printf("start mr_new\n");
    //printf("%s: new Manager %d\n", currentThread->getName(), i);
    //mr[i] = new Manager(i);
    test1();
    mr[0]->work();
    
}
void mr_new_testcase(int i) {
    //printf("start mr_new\n");
    //printf("%s: new Manager %d\n", currentThread->getName(), i);
    //mr[i] = new Manager(i);
    if(test_case()){
        mr[0]->work();
    }
}
bool test_case(){

    printf ("Enter [ticket clerk] [concession clerk] [ticket taker] and [max customer] number:");
    scanf ("%d %d %d %d",&tcNumber,&ccNumber,&ttNumber,&crNumber);
    if(tcNumber>MAX_TC||tcNumber<=0){
        printf ("ticket clerk number should be between 0 and 5\n");
        return false; 
    }   
    //printf ("Enter your concession clerk number: ");
    //scanf ("%d",&ccNumber);
    if(ccNumber>MAX_CC||ccNumber<=0){
        printf ("Concession clerk number should be between 0 and 5\n");
        return false; 
    }   
   // printf ("Enter your ticket taker number: ");
   // scanf ("%d",&ttNumber);
    if(ttNumber>MAX_TT||ttNumber<=0){
        printf ("Ticket taker number should be between 0 and 3\n");
        return false; 
    }  
   // printf ("Enter your max customer number: ");
   // scanf ("%d",&groupSum);
    if(crNumber>MAX_CR_NUMBER){
        printf ("Customer should be less than 100\n");
        return false; 
    } 
   
    
    printf("Number of TicketClerks = [%d]\n",tcNumber);
    printf("Number of ConcessionClerks = [%d]\n",ccNumber); 
    printf("Number of TicketTakers = [%d]\n",ttNumber);




     init();
   char *name;
    Thread * t; 


    // create TicketClerk
    for (int i = 0;i < tcNumber; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_TicketClerk_%d", i); 
        // ? in thread or hear ?
//        tc[i] = new TicketClerk(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)tc_new, i);
    }




  // create Customer
    //groupSum = rand()%10 + 10;//MIN_GROUP + MIN_GROUP;
    groupSum=0;
    nextCustomerNumber = 0; 
   for (int i = 0;i < MAX_GP_NUMBER; ++i) {
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
        groupSum++;
        if (nextCustomerNumber >= crNumber && nextCustomerNumber >= MIN_CR) break;
        
    }
    printf("Number of Groups = [%d]\n",groupSum);
    printf("Number of Customers = [%d]\n",nextCustomerNumber);

    lCustomerLeft->Acquire();
    customerLeft = nextCustomerNumber;
    lCustomerLeft->Acquire();
    
    // create ConcessionClerk 
    for (int i = 0;i < ccNumber; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_ConcessionClerk_%d", i);
        // ? in thread or here
//        cc[i] = new ConcessionClerk(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)cc_new, i);
    }
    
    // create TicketTaker 
    for (int i = 0;i < ttNumber; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_TicketTaker_%d", i);
        // ? in thread or here
//        tt[i] = new TicketTaker(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)tt_new, i);
    }
    

    // create MovieTechnician 
    for (int i = 0;i < MAX_MT; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_MovieTechnician_%d", i);
        // ? in thread or here
//        mt[i] = new MovieTechnician(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mt_new, i);
    }


}

void test1() {

    tcNumber= MAX_TC;
    ccNumber= MAX_CC;
    ttNumber= MAX_TT;
   


   
    printf("Number of TicketClerks = [%d]\n",tcNumber);
    printf("Number of ConcessionClerks = [%d]\n",ccNumber); 
    printf("Number of TicketTakers = [%d]\n",ttNumber);
    


    char *name;
    Thread *t;

    // create TicketClerk
    for (int i = 0;i < tcNumber; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_TicketClerk_%d", i); 
        // ? in thread or hear ?
//        tc[i] = new TicketClerk(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)tc_new, i);
    }
    
    // create Customer
    groupSum = rand()%10 + 10;//MIN_GROUP + MIN_GROUP;
    nextCustomerNumber = 0; 
   for (int i = 0;i < groupSum; ++i) {
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
    printf("Number of Groups = [%d]\n",groupSum);
    printf("Number of Customers = [%d]\n",nextCustomerNumber);

    lCustomerLeft->Acquire();
    customerLeft = nextCustomerNumber;
    lCustomerLeft->Acquire();
    
    // create ConcessionClerk 
    for (int i = 0;i < ccNumber; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_ConcessionClerk_%d", i);
        // ? in thread or here
//        cc[i] = new ConcessionClerk(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)cc_new, i);
    }
    
    // create TicketTaker 
    for (int i = 0;i < ttNumber; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_TicketTaker_%d", i);
        // ? in thread or here
//        tt[i] = new TicketTaker(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)tt_new, i);
    }
    

    // create MovieTechnician 
    for (int i = 0;i < MAX_MT; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_MovieTechnician_%d", i);
        // ? in thread or here
//        mt[i] = new MovieTechnician(i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mt_new, i);
    }
     
}
/*void test1() {
    char *name;
    Thread *t;
      tcNumber= MAX_TC;
    ccNumber= MAX_CC;
    ttNumber= MAX_TT; 


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
    groupSum = rand()%5 + 10;//MIN_GROUP + MIN_GROUP;
    nextCustomerNumber = 0; 
    for (int i = 0;i < groupSum; ++i) {
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
    lCustomerLeft->Acquire();
    customerLeft = nextCustomerNumber;
    lCustomerLeft->Acquire();
    printf("Number of Groups = [%d]\n",groupSum);
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
     
}*/
void Problem2() {
    init();
    char* name;
    Thread * t; 
    // create Manager 
    for (int i = 0;i < MAX_MR; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Manager_%d", i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mr_new, i);
    }

}



void cr_new_t1(int arg) {
    
   CustomerData *customerData = (CustomerData *) arg; 
    
}

void cr_tb_new_t1(int arg) {
       
    CustomerData *customerData = (CustomerData *) arg;  

    cr[customerData->customerNumber]->buyTickets();

}



//    * Customers always take the shortest line, but no 2 customers ever choose the same shortest line at the same time
//    * Managers only read one from one Clerk's total money received, at a time.
//    * Customers do not leave a Clerk, or TicketTaker, until they are told to do so. Clerks and TicketTakers do not start with another Customer until they know the current Customer has left. customer until they know that the last Customer has left their area
//    * Managers get Clerks off their break when lines get too long
//    * Total sales never suffers from a race condition
//    * Customer groups always move together through the theater. This requires explicit synchronization that you implement.

void TestCase_P2_1(){

   init();
   char *name;
    Thread * t; 

   
    // create Manager 
    
    for (int i = 0;i < MAX_MR; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Manager_%d", i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mr_new_testcase, i);
    }
 











    // create TicketClerk
    /*for (int i = 0;i < MAX_TC; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_TicketClerk_%d", i); 
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)tc_new, i);
    }
    printf("Number of TicketClerks = [%d]\n",MAX_TC);
    // create Customer
    //groupSum = rand()%MAX_GROUP + 1;
    nextCustomerNumber = 0; 
    for (int i = 0;i < MAX_GROUP; ++i) {
        int groupSize = rand()%MAX_GROUPSIZE+1;
        for (int j = 0;j < groupSize - 1; ++j) {
            name = new char[MAX_VAR];
            sprintf(name, "Thread_Customer_%d", nextCustomerNumber);
            CustomerData *customerData = new CustomerData;
            customerData->customerNumber = nextCustomerNumber++;
            customerData->groupNumber = i;
            customerData->groupSize = groupSize;
            cr[customerData->customerNumber] = new Customer(customerData->customerNumber, customerData->groupNumber, customerData->groupSize, false);
            t = new Thread(name);
            t->Fork((VoidFunctionPtr)cr_new_t1, int(customerData));
        }
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Head_Customer_%d", nextCustomerNumber);
        CustomerData *customerData = new CustomerData;
        customerData->customerNumber = nextCustomerNumber++;
        customerData->groupNumber = i;
        customerData->groupSize = groupSize;
        cr[customerData->customerNumber] = new Customer(customerData->customerNumber, customerData->groupNumber, customerData->groupSize, true);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)cr_tb_new_t1, int(customerData));
        //if (nextCustomerNumber >= cin && nextCustomerNumber >= 50) break;
    }
    printf("Number of Customers = [%d]\n",nextCustomerNumber);
    customerLeft = nextCustomerNumber;
    printf("Number of Groups = [%d]\n",groupSum); */
   
}

void TestCase_P2_2(){

   init();
   char *name;
    Thread * t; 
    // create Manager 
    
    for (int i = 0;i < MAX_MR; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Manager_%d", i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mr_new, i);
    }

   
}


void TestCase_P2_3(){

   init();
   char *name;
    Thread * t; 
    // create Manager 
    
    for (int i = 0;i < MAX_MR; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Manager_%d", i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mr_new, i);
    }

   
}

void mr_t4() {
    while (true) {
        mr[0]->randToBreak(lBuyTickets, (Employee**)tc, MAX_TC, noTicketClerk, cNoTicketClerk, sNoTicketClerk);
        for (int i = 0;i < 10; ++i) {
            currentThread->Yield();
        }
    }
}
void TestCase_P2_4(){

    init();
    char *name;
    Thread * t; 

    // set ticketClerk on break
    for (int i = 1; i < MAX_TC; ++i) {
        tc[i]->setIsBreak(true);
    }

    // create Manager 
    for (int i = 0;i < MAX_MR; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Manager_%d", i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mr_t4, i);
    }
    test1();
   
}
void TestCase_P2_5(){

   init();
   char *name;
    Thread * t; 
    // create Manager 
    
    for (int i = 0;i < MAX_MR; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Manager_%d", i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mr_new, i);
    }

   
}
void TestCase_P2_6(){

   init();
   char *name;
    Thread * t; 
    // create Manager 
    
    for (int i = 0;i < MAX_MR; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Manager_%d", i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)mr_new, i);
    }

   
}
