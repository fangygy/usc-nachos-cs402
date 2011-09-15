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

extern Lock *lBuyTickets;
extern Lock *lBuyFood;
extern Lock *lCheckTickets;

class Employee {
  private:
    bool isBusy;
    bool isBreak;
    int id;
    char employeeType[MAX_VAR];
    int waitingSize;
  protected:
    void setIsBusy(bool busy);
    void setIsBreak(bool isBreak);
  public:
    Lock *lock;
    Condition *condition[2];
    bool getIsBusy();
    bool getIsBreak();
    int getId();
    Employee(int eId, char *type);
    ~Employee();
    char * getEmployeeType();
    int getWaitingSize();
    void addWaitingSize();
    void subWaitingSize();
};

class Customer {
  private:
    int customerId;
    int groupId;
    bool isTicketBuyer;
    int groupSize;   //Only head customers need this value
// how to find groupmate
    Customer * customers;   //Only head customers need this value
    void printTCStatus();
  public:
    // customerId, groupId, isTicketBuyer
    Customer(int cId, int gId, int gSize, bool ticketBuyer);
    ~Customer();
    void action();
    bool getIsTicketBuyer();
    // for ticketBuyer to buy tickets from ticketClerk
    void buyTickets();

    // for ticketBuyer to buy food from ConcessionClerk
    void buyFood();
    // for ticketBuyer to count Food
    int countFood(char * food);
    // for ticketBuyer to ask if buy a food
    bool askForFood(char * food);
    // for customer to decide if buy a food
    bool answerForFood(char * food);

    // for ticketBuyer to check ticket
    void checkTickets();

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
    int ticketSum;
    double amount;
    double payment;
    void printStatus();
  public:
    TicketClerk(int tcId);
    void sellTickets();
    void setTicketSum(int sum);
    int getTicketSum();
    void setAmount(double amount);
    double getAmount();
    void setPayment(double amount);
};
extern TicketClerk *tc[MAX_TC];

class ConcessionClerk : public Employee{
  private:
    int food[2];
    double amount;
    double payment;
    void printStatus();
  public:
    ConcessionClerk(int ccId);
    void sellFood();
    void setFood(int pos, int sum);
    int getFood(int pos);
    void setAmount(double amount);
    double getAmount();
    void setPayment(double amount);
    double getPayment();
 	
};
extern ConcessionClerk *cc[MAX_CC];

class TicketTaker : public Employee{
  private:
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

#endif
