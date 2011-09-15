#ifndef MOVIETHEATER
#define MOVIETHEATER

#include <stdlib.h>
#include "system.h"
#include "synch.h"

#define MIN_CR 40  // Customers
#define MAX_TC 5  // TicketClerks
#define MAX_CC 5  // ConcessionClerks
#define MAX_TT 3  // TicketTakers
#define MAX_MT 1  // MovieTechnician
#define MAX_MR 1  // Manager
#define MAX_GROUP 10  // Group
#define MAX_GROUPSIZE 5  // groupsize

#define MAX_VAR 200

#define RATE_POPCORN 0.75  // chance to buy Popcorn
#define RATE_SODA 0.75  // chance to buy Soda
#define RATE_BATHROOM 0.25  // chance to go bathroom

extern Lock *lBuyTickets;  //Lock to get in line of buyTickets
extern Lock *lBuyFood;  //Lock to get in line of buyFood
extern Lock *lCheckTickets;  //Lock to get in line of checkTickets
extern Lock *lStartMovie;  //Lock to get seats and startMovie
extern Semaphore *sGroup[MAX_GROUP];  //semaphore to make group act together
extern Condition *cGroup[MAX_GROUP];  //Condition to make group act together
extern Condition *cGroupFood[MAX_GROUP];  //Condition for ticketbuyer to ask for if need food
extern Lock *lGroup[MAX_GROUP];  //Lock for group to act mutex
extern int groupFoodSum[MAX_GROUP][2];  // order of group for food
extern int groupTicketSum[MAX_GROUP];  // order of group for tickets
extern Semaphore *sGroupFood[MAX_GROUP];  //semaphore to make sure everyone choose if need food
extern bool groupTicket[MAX_GROUP];  // monitor variable for if buyTickets done
extern bool groupAskForFood[MAX_GROUP];  // monitor variable for if ask for food
extern bool groupFood[MAX_GROUP];  // monitor variable for if buyFood done
extern bool groupSeat[MAX_GROUP];  // monitor variable for if get seats

extern bool seatState[25];

class Employee {
  private:
    bool isBusy;
    bool isBreak;
    int id;
    char employeeType[MAX_VAR];
    // customer in waiting line
    int waitingSize;
  protected:
    void setIsBusy(bool busy);
    void setIsBreak(bool isBreak);
  public:
    // for action between with customer and employee
    Lock *lock;
    // c0 is for customer in waiting line
    // c1 is action between customer and employee
    Condition *condition[2];
    bool getIsBusy();
    bool getIsBreak();
    int getId();
    Employee(int eId, char *type);
    ~Employee();
    char * getEmployeeType();
    int getWaitingSize();
    // waitingSize add 1
    void addWaitingSize();
    // waitingSize sub 1
    void subWaitingSize();
};

class Customer {
  private:
    int customerId;
    int groupId;
    bool isTicketBuyer;
    int groupSize;   //Only head customers need this value
    // how to find groupmate
    //    Customer * customers;   //Only head customers need this value
    void printTCStatus();
    int seatNumber;
  public:
    // customerId, groupId, isTicketBuyer
    Customer(int cId, int gId, int gSize, bool ticketBuyer);
    ~Customer();
    // customer action for ticketBuyer and regular Customer
    void action();
    bool getIsTicketBuyer();
    // for ticketBuyer to buy tickets from ticketClerk
    void buyTickets();

    // for ticketBuyer to buy food from ConcessionClerk
    void buyFood();
    // for ticketBuyer to count Food
    bool countFood();
    // for customer to decide if buy a food
    int answerForFood(int food);

    // for ticketBuyer to check ticket
    void checkTickets();
    // arrange seats
    void arrangeSeats();
    // for regular custormer
    // wait for buy tickets done
    void waitTickets();
    // wait for buy food done
    void waitFood();
    // wait for take seats done
    void waitSeats();
    // wait for group act together
    void waitGroup();
    void setSeatNumber(int seatNum);

    // for customer to go bathroom
    void goBathroom();
    // for ticketBuyer to check group is ready to leave
    void leaveTheater();
    // find and get in the line
    int getInLine(Lock * lock, Employee** employee, int count);
};
extern Customer *cr[MIN_CR]; 

class TicketClerk : public Employee{
  private:
    // customer sum in one group
    int ticketSum;
    // amount in one group
    double amount;
    // customer pay
    double payment;
    void printStatus();
  public:
    TicketClerk(int tcId);
    void sellTickets();
    void setTicketSum(int sum);
    int getTicketSum();
    // put amount in the variable
    void setAmount(double amount);
    // get amount
    double getAmount();
    // receive money from customer
    void setPayment(double amount);
};
extern TicketClerk *tc[MAX_TC];

class ConcessionClerk : public Employee{
  private:
    // sum for each food in one group
    int food[2];
    // amount for food
    double amount;
    // customer pay
    double payment;
    void printStatus();
  public:
    ConcessionClerk(int ccId);
    void sellFood();
    // customer tell cc sum for each food
    void setFood(int pos, int sum);
    int getFood(int pos);
    // amount for the food
    void setAmount(double amount);
    double getAmount();
    void setPayment(double amount);
    double getPayment();
 	
};
extern ConcessionClerk *cc[MAX_CC];

class TicketTaker : public Employee{
  private:
    // tickets sum one group has
    int ticketSum; 
  public:
    TicketTaker(int ttId);
    void checkTickets(); 
    void setTicketSum(int sum);
    int getTicketSum();
};
extern TicketTaker * tt[MAX_TT];

class MovieTechnician : public Employee{
  private:
    
  public:
    MovieTechnician(int mtId);
};
extern MovieTechnician * mt[MAX_MT];
class Manager : public Employee{
  private:
    
  public:
    Manager(int mrId);
};
extern Manager * mr[MAX_MR];

void init();
#endif
