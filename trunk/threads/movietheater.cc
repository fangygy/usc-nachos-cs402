#include "movietheater.h"

// declare pointer
Customer *cr[MIN_CR];
TicketClerk *tc[MAX_TC];
ConcessionClerk *cc[MAX_CC];
TicketTaker * tt[MAX_TT];
MovieTechnician * mt[MAX_MT];
Manager * mr[MAX_MR];

Lock *lBuyTickets = new Lock("Lock_BuyTickets_line");
Lock *lBuyFood = new Lock("Lock_BuyFood_line");;
Lock *lCheckTickets = new Lock("Lock_CheckTickets_line");;



