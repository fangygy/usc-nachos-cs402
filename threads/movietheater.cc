#include "movietheater.h"

// declare pointer
Customer *cr[MAX_CR];
TicketClerk *tc[MAX_TC];
ConcessionClerk *cc[MAX_CC];
TicketTaker * tt[MAX_TT];
MovieTechnician * mt[MAX_MT];
Manager * mr[MAX_MR];

Lock *lBuyTickets = new Lock("Lock_BuyTickets_line");
Lock *lBuyFood = new Lock("Lock_BuyFood_line");
Lock *lCheckTickets = new Lock("Lock_CheckTickets_line");
Lock *lStartMovie = new Lock("Lock_StartMovie");
Lock *lStopMovie = new Lock("Lock_StopMovie");
Lock *lFindSeats = new Lock("Lock_FindSeat");
Lock *lTicketSold = new Lock("Lock_TicketSold");
Lock *lTicketTaken = new Lock("Lock_TicketTaken");
Lock *lMovieState = new Lock("Lock_lMovieState");
Lock *lAmount = new Lock("Lock_Amount"); 
Lock *lCustomerLeft = new Lock("Lock_CustomerLeft");
Semaphore *sGroup[MAX_GROUP];
Condition *cGroup[MAX_GROUP];
Condition *cGroupFood[MAX_GROUP];
Condition *cLeaveBathroomGroup[MAX_GROUP]; 
Condition *cTicketTaken = new Condition("Condition_TicketTaken");
Lock *lGroup[MAX_GROUP];
Lock *lBathroomNum[MAX_GROUP]; 
int groupFoodSum[MAX_GROUP][2];
int groupBathroomSum[MAX_GROUP];
int ticketReceipt[MAX_GROUP];
int ticketTaken = 0;
bool stopTicketTaken = false;  // all stop
bool stopTicketTakenArr[MAX_TT];  // each tt stop
Semaphore *sGroupFood[MAX_GROUP];
Semaphore *sGroupBathroom[MAX_GROUP];
Semaphore *sGroupLeaveBathroom[MAX_GROUP]; 
bool groupTicket[MAX_GROUP];
bool groupAskForFood[MAX_GROUP];
bool groupAskForBathroom[MAX_GROUP];
bool groupFood[MAX_GROUP];
bool groupSeat[MAX_GROUP];
bool groupLeaveRoom[MAX_GROUP];
bool groupLeaveTheater[MAX_GROUP];
bool groupLeaveBathRoom[MAX_GROUP];
extern bool groupLeaveTheater[MAX_GROUP];
int movieState;  //monitor variable for Movies State, 0: over, 1: not start, 2: start
int seatPos=-1;
bool seatState[MAX_SEAT];
Semaphore *sWaitSeat[MAX_GROUP]; 
int SeatLocation[MAX_GROUPSIZE];
Semaphore *sStartMovie = new Semaphore("Semaphore_StartMovie",0);
Semaphore *sMT_CR_Check = new Semaphore("Semaphore_CheckCustomerSeated",0);
Condition *cMT_CR_Check = new Condition("Condition_CheckCustomerSeated");
Semaphore *sStopMovie = new Semaphore("Semaphore_StopMovie",0);
Semaphore *sMT_CR_Stop = new Semaphore("Semaphore_WakeCustomer",0);
Condition *cMT_CR_Stop = new Condition("Condition_WakeCustomer");

bool noTicketClerk = false;
bool noConcessionClerk = false;
bool noTicketTaker = false;
Condition *cNoTicketClerk = new Condition("Condition_NoTicketClerk");
Condition *cNoConcessionClerk = new Condition("Condition_NoConcessionClerk");;
Condition *cNoTicketTaker = new Condition("Condition_NoTicketTaker");;
Semaphore *sNoTicketClerk = new Semaphore("Semaphore_NoTicketClerk",0);
Semaphore *sNoConcessionClerk = new Semaphore("Semaphore_NoConcessionClerk",0);
Semaphore *sNoTicketTaker = new Semaphore("Semaphore_NoTicketTaker",0);

int nextCustomerNumber;
int groupSum;
int totalTicketSold;
int totalTicketTaken;
double totalAmount;
double ticketClerkAmount[MAX_TC];
double concessionClerkAmount[MAX_CC];
int customerLeft = -1;
int tcNumber=-1,ccNumber=-1,ttNumber=-1,crNumber=-1;

void init() {
    //initial all value
    memset(groupFoodSum, 0, sizeof(groupFoodSum));
    memset(ticketReceipt, 0, sizeof(ticketReceipt));
    memset(groupTicket, 0, sizeof(groupTicket));
    memset(groupAskForFood, 0, sizeof(groupAskForFood));
    memset(groupFood, 0, sizeof(groupFood));
    memset(groupSeat, 0, sizeof(groupSeat));
    memset(groupLeaveRoom, 0, sizeof(groupLeaveRoom));
    memset(groupAskForBathroom, 0, sizeof(groupAskForBathroom));
    memset(groupBathroomSum, 0, sizeof(groupBathroomSum));
    memset(groupLeaveTheater, 0, sizeof(groupLeaveTheater));

 
  
    mr[0] = new Manager(0);
    int i;
    //initial
    for (i = 0;i < MAX_GROUP; ++i) {
        sGroup[i] = new Semaphore("Semaphore_Group", 0);
        sGroupFood[i] = new Semaphore("Semaphore_GroupFood", 0);
        sGroupBathroom[i] = new Semaphore("Semaphore_GroupBathroom", 0);
        sGroupBathroom[i] = new Semaphore("Semaphore_GroupBathroom", 0);
        sGroupLeaveBathroom[i] = new Semaphore("Semaphore_GroupLeaveBathroom", 0);
        cGroup[i] = new Condition("Condition_Group");
        cGroupFood[i] = new Condition("Condition_GroupFood");
        lGroup[i] = new Lock("Lock_Group");
        lBathroomNum[i]= new Lock("Lock_BathroomNumber");
        sWaitSeat[i]= new Semaphore("Semaphore_WaitSeat", 0); 

    }
     //initial seat state
    for(i=0;i<MAX_SEAT;i++){
         seatState[i]=false;  
    }

    //new ticket clerk
    for (i = 0;i < MAX_TC; ++i) {
        tc[i] = new TicketClerk(i);
    }
    //new concession clerk
    for (i = 0;i < MAX_CC; ++i) {
        cc[i] = new ConcessionClerk(i);
    }
    //new ticket taker
    for (i = 0;i < MAX_TT; ++i) {
        stopTicketTakenArr[i] = false;
        tt[i] = new TicketTaker(i);
    }
    //new movie technician
    for (i = 0;i < MAX_MT; ++i) {
        mt[i] = new MovieTechnician(i);
    }


}

