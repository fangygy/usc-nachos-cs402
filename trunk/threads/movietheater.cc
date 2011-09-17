#include "movietheater.h"

// declare pointer
Customer *cr[MIN_CR];
TicketClerk *tc[MAX_TC];
ConcessionClerk *cc[MAX_CC];
TicketTaker * tt[MAX_TT];
MovieTechnician * mt[MAX_MT];
Manager * mr[MAX_MR];

Lock *lBuyTickets = new Lock("Lock_BuyTickets_line");
Lock *lBuyFood = new Lock("Lock_BuyFood_line");
Lock *lCheckTickets = new Lock("Lock_CheckTickets_line");
Lock *lStartMovie = new Lock("Lock_StartMovie");
Lock *lFindSeats = new Lock("Lock_FindSeat");
Semaphore *sGroup[MAX_GROUP];
Condition *cGroup[MAX_GROUP];
Condition *cGroupFood[MAX_GROUP];
Lock *lGroup[MAX_GROUP];
int groupFoodSum[MAX_GROUP][2];
int ticketReceipt[MAX_GROUP];
Semaphore *sGroupFood[MAX_GROUP];
bool groupTicket[MAX_GROUP];
bool groupAskForFood[MAX_GROUP];
bool groupFood[MAX_GROUP];
bool groupSeat[MAX_GROUP];
int ticketToken;
int seatPos=-1;
bool seatState[MAX_SEAT];
Semaphore *sSeat[MAX_SEAT];
Semaphore *sWaitSeat[MAX_GROUP]; 
int SeatLocation[MAX_GROUPSIZE];



void init() {
    memset(groupFoodSum, 0, sizeof(groupFoodSum));
    memset(ticketReceipt, 0, sizeof(ticketReceipt));
    memset(groupTicket, 0, sizeof(groupTicket));
    memset(groupAskForFood, 0, sizeof(groupAskForFood));
    memset(groupFood, 0, sizeof(groupFood));
    memset(groupSeat, 0, sizeof(groupSeat));
  
    mr[0] = new Manager(0);
    int i;
    for (i = 0;i < MAX_GROUP; ++i) {
        sGroup[i] = new Semaphore("Semaphore_Group", 0);
        sGroupFood[i] = new Semaphore("Semaphore_GroupFood", 0);
        cGroup[i] = new Condition("Condition_Group");
        cGroupFood[i] = new Condition("Condition_GroupFood");
        lGroup[i] = new Lock("Lock_Group");
        sWaitSeat[i]= new Semaphore("Semaphore_WaitSeat", 0); 
    }

    for(i=0;i<MAX_SEAT;i++){
         sSeat[i] = new Semaphore("Semaphore_Seat", 0);
         seatState[i]=false;  
    }


    for (i = 0;i < MAX_TC; ++i) {
        tc[i] = new TicketClerk(i);
    }

    for (i = 0;i < MAX_CC; ++i) {
        cc[i] = new ConcessionClerk(i);
    }

    for (i = 0;i < MAX_TT; ++i) {
        tt[i] = new TicketTaker(i);
    }

    for (i = 0;i < MAX_MT; ++i) {
        mt[i] = new MovieTechnician(i);
    }


}

