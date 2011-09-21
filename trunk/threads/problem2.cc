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
bool test_case(int i);
void TestCase_P2(int i);
void createClerk();

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
void testcase(int i) {
    if(test_case(i)){
        mr[0]->work();
    }
}
//    * Customers always take the shortest line, but no 2 customers ever choose the same shortest line at the same time
//    * Managers only read one from one Clerk's total money received, at a time.
//    * Customers do not leave a Clerk, or TicketTaker, until they are told to do so. Clerks and TicketTakers do not start with another Customer until they know the current Customer has left. customer until they know that the last Customer has left their area
//    * Managers get Clerks off their break when lines get too long
//    * Total sales never suffers from a race condition
//    * Customer groups always move together through the theater. This requires explicit synchronization that you implement.
bool test_case(int i){
    init();
    printf ("Please enter [ticket clerk (1 to 5)] [concession clerk(1 to 5)] [ticket taker( 1 to 3)] and [MAX customer( 1 to 100)] number.\n");

    if (i == 1) {
        printf(" Press <ENTER> to use default data: 2 2 2 20\n");
        printf(" Test1: Customers always take the shortest line, but no 2 customers ever choose the same shortest line at the same time.\n");
        printf(" Result for default data: customers will wait in different line by turns.\n");
        tcNumber = 2;
        ccNumber = 2;
        ttNumber = 2;
        crNumber = 20;
    } else if (i == 2) {
        printf(" Press <ENTER> to use default data: 4 4 2 30\n");
        printf(" Test2: Managers only read one from one Clerk's total money received, at a time.\n");
        printf(" Result for default data: total money equals to the sum of all the clerk.\n");
        tcNumber = 4;
        ccNumber = 4;
        ttNumber = 2;
        crNumber = 30;
    } else if (i == 3) {
        printf(" Press <ENTER> to use default data: 1 1 1 10\n");
        printf(" Test3: Customers do not leave a Clerk, or TicketTaker, until they are told to do so. Clerks and TicketTakers do not start with another Customer until they know the current Customer has left. customer until they know that the last Customer has left their area.\n");
        printf(" Result for default data: output as the test description.\n");
        tcNumber = 1;
        ccNumber = 1;
        ttNumber = 1;
        crNumber = 10;
    } else if (i == 4) {
        printf(" Press <ENTER> to use default data: 2 1 1 30\n");
        printf(" Test4: Managers get Clerks off their break when lines get too long.\n");
        printf(" Result for default data: init one ticket clerk on break, manager will wait him up when lines get more than 5.\n");
        tc[0]->setIsBreak(true);
        tcNumber = 2;
        ccNumber = 1;
        ttNumber = 1;
        crNumber = 30;
    } else if (i == 5) {
        printf(" Press <ENTER> to use default data: 5 5 3 40\n");
        printf(" Test5: Total sales never suffers from a race condition.\n");
        printf(" Result for default data: ticket clerk sales equals to sum(customer)*12, concession clerk sales equals to sum(soda)*4+sum(popcorn)*5. Total sales equals to clerk sales sum.\n");
        tcNumber = 5;
        ccNumber = 5;
        ttNumber = 3;
        crNumber = 40;
    } else if (i == 6) {
        printf(" Press <ENTER> to use default data: 2 2 2 20\n");
        printf(" Test6: Customer groups always move together through the theater. This requires explicit synchronization that you implement.\n");
        printf(" Result for default data: customers will wait headcustomer to gather and proceed in group.\n");
        tcNumber = 2;
        ccNumber = 2;
        ttNumber = 2;
        crNumber = 20;
    }
    char line[MAX_VAR];
    gets(line);
    if (strlen(line) != 0) {
        sscanf (line, "%d %d %d %d",&tcNumber,&ccNumber,&ttNumber,&crNumber);
        if(tcNumber>MAX_TC||tcNumber<=0){
            printf ("ticket clerk number should be between 1 and 5\n");
            return false; 
        }   
        //printf ("Enter your concession clerk number: ");
        //scanf ("%d",&ccNumber);
        if(ccNumber>MAX_CC||ccNumber<=0){
            printf ("Concession clerk number should be between 1 and 5\n");
            return false; 
        }   
       // printf ("Enter your ticket taker number: ");
       // scanf ("%d",&ttNumber);
        if(ttNumber>MAX_TT||ttNumber<=0){
            printf ("Ticket taker number should be between 1 and 3\n");
            return false; 
        }  
       // printf ("Enter your max customer number: ");
       // scanf ("%d",&groupSum);
        if(crNumber>MAX_CR_NUMBER){
            printf ("Customer should be less than 100\n");
            return false; 
        } 
    }
    createClerk();
    return true;
}
    
    
void createClerk() {
    printf("Number of TicketClerks = [%d]\n",tcNumber);
    printf("Number of ConcessionClerks = [%d]\n",ccNumber); 
    printf("Number of TicketTakers = [%d]\n",ttNumber);
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
        if (nextCustomerNumber >= crNumber || nextCustomerNumber >= MIN_CR) break;
        
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



void TestCase_P2(int index){

   init();
   char *name;
    Thread * t; 

   
    // create Manager 
    
    for (int i = 0;i < MAX_MR; ++i) {
        name = new char[MAX_VAR];
        sprintf(name, "Thread_Manager_%d", i);
        t = new Thread(name);
        t->Fork((VoidFunctionPtr)testcase, index);
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
