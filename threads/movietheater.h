#ifndef MOVIETHEATER
#define MOVIETHEATER

#include <stdlib.h>
#include "system.h"
#include "synch.h"

#define MAX_CR 1000
#define MIN_CR 40  // Customers
#define MAX_TC 5  // TicketClerks
#define MAX_CC 5  // ConcessionClerks
#define MAX_TT 3  // TicketTakers
#define MAX_MT 1  // MovieTechnician
#define MAX_MR 1  // Manager
#define MAX_GROUP 20  // Group
#define MIN_GROUP 10  // Group
#define MAX_GROUPSIZE 5  // groupsize

#define MAX_VAR 200

#define RATE_POPCORN 0.75  // chance to buy Popcorn
#define RATE_SODA 0.75  // chance to buy Soda
#define RATE_BATHROOM 0.25  // chance to go bathroom

#define PRICE_TICKET 12.00
#define PRICE_SODA 4.00
#define PRICE_POPCORN 5.00

#define FOOD_POPCORN 0
#define FOOD_SODA    1 

#define MOVIE_PERIOD_BASE  200
#define MOVIE_PERIOD_VAR  100

#define MAX_SEAT   25
#define MAX_ROW    5
#define MAX_COL    5

extern Lock *lBuyTickets;  //Lock to get in line of buyTickets
extern Lock *lBuyFood;  //Lock to get in line of buyFood
extern Lock *lCheckTickets;  //Lock to get in line of checkTickets
extern Lock *lStartMovie;  //Lock to get seats and startMovie
extern Lock *lStopMovie; //stop Movie
extern Lock *lFindSeats;
extern Lock *lGroup[MAX_GROUP];  //Lock for group to act mutexnn
extern Lock *lTicketSold;  // Lock for ticket sold
extern Lock *lTicketTaken;  // lock for ticket taken
extern Lock *lAmount;  // lock for money collect
extern Lock *lCustomerLeft;  // lock for leave sum

extern Semaphore *sGroup[MAX_GROUP];  //semaphore to make group act together
extern Semaphore *sGroupFood[MAX_GROUP];  //semaphore to make sure everyone choose if need food
extern Semaphore *sGroupBathroom[MAX_GROUP];  //semaphore to make sure everyone choose if need food
extern Semaphore *sGroupLeaveBathroom[MAX_GROUP];  //semaphore to make sure everyone choose if need Bathroom
extern Condition *cGroup[MAX_GROUP];  //Condition to make group act together
extern Condition *cLeaveBathroomGroup[MAX_GROUP];  //Condition to make group act together
extern Condition *cGroupFood[MAX_GROUP];  //Condition for ticketbuyer to ask for if need food
//extern Condition *cGroupBathroom[MAX_GROUP];  //Condition for ticketbuyer to ask for if need bathroom
extern Condition *cTicketTaken;  // Condition for ticketTaker and customer to know start ticket taken 

extern int groupFoodSum[MAX_GROUP][2];  // order of group for food
extern int groupBathroomSum[MAX_GROUP];  // order of group for food
extern int ticketReceipt[MAX_GROUP];  // order of group for tickets
extern int ticketTaken;  // ticket taken num
extern bool stopTicketTaken;  // monitor variable for stop ticketTaken
extern bool groupTicket[MAX_GROUP];  // monitor variable for if buyTickets done
extern bool groupAskForFood[MAX_GROUP];  // monitor variable for if ask for food
extern bool groupAskForBathroom[MAX_GROUP];  // monitor variable for if ask for food
extern bool groupFood[MAX_GROUP];  // monitor variable for if buyFood done
extern bool groupSeat[MAX_GROUP];  // monitor variable for if get seats
extern bool groupArrangeSeat[MAX_GROUP];  // monitor variable for if get seats
extern bool groupLeaveRoom[MAX_GROUP];
extern bool groupLeaveTheater[MAX_GROUP];

extern int movieState;  //monitor variable for Movies State, 0: over, 1: not start, 2: start
extern Lock *lMovieState;
extern bool seatState[MAX_SEAT];
extern Semaphore *sSeat[MAX_SEAT];  //semaphore to Seat
extern int ticketTaken;
extern int totalTicketTaken;
extern int totalTicketSold;
extern double totalAmount;
extern double ticketClerkAmount[MAX_TC];
extern double concessionClerkAmount[MAX_CC];
extern int seatPos;

extern bool noTicketClerk;
extern bool noConcessionClerk;
extern bool noTicketTaker;
extern Condition *cNoTicketClerk;
extern Condition *cNoConcessionClerk;
extern Condition *cNoTicketTaker;
extern Semaphore *sNoTicketClerk;
extern Semaphore *sNoConcessionClerk;
extern Semaphore *sNoTicketTaker;


extern Semaphore *sWaitSeat[MAX_GROUP];  //semaphore to Seat
extern int SeatLocation[MAX_GROUPSIZE];
extern Semaphore *sStartMovie; 
extern Semaphore *sMT_CR_Check;
extern Condition *cMT_CR_Check;

extern Semaphore *sStopMovie; 
extern Semaphore *sMT_CR_Stop;
extern Condition *cMT_CR_Stop;

extern int nextCustomerNumber;
extern int groupSum;
extern int customerLeft;

class Employee {
  private:
    bool isBusy;
    bool isBreak;
    int id;
    // group on service
    int groupId;
    char employeeType[MAX_VAR];
    // customer in waiting line
    int waitingSize;
  public:
    void setIsBusy(bool busy);
    void setIsBreak(bool isBreak);
    // for action between with customer and employee
    Lock *lock;
    // c0 is for customer in waiting line
    // c1 is action between customer and employee
    Condition *condition[3];
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
    // customer group on service
    void setGroupId(int gId);
    int getGroupId();
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
    //Head Customer get Seat
    int getSeats(int Size);

    // for regular custormer
    // wait for buy tickets done
    void waitTickets();
    // wait for buy food done
    void waitFood();
    // wait for take seats done
    void waitSeats();
    // wait for ticket check done
    void waitCheck();
    // wait for group act together
    void waitGroup();
    // wait for group to leave theater room
    void waitLeaveRoom(); 
    // ticketbuyer tell customer to proceed
    void proceed(bool * flag);
    void setSeatNumber(int seatNum);

    // for customer to go bathroom
    void goBathroom();
    // for ticketBuyer to check group is ready to leave
    void leaveTheater();
    // find and get in the line
    int getInLine(Lock *lock, Condition *cNoClerk, int count, bool noClerk, Employee** employee);
    // watch movies when all customer seated
    void watchMovie();
    void prtLeaveRoomMsg();
    // head customer check if any one wants go bathroom
    void checkBathroom();
   // customer go to bathroom or wait in the lobby    
    void waitBathroom();
     // for customer to decide if want to go bathroom
    bool answerForBathroom();
    void waitLeaveTheater();
    int getGroupId(); 
    int getId();   
     

};
extern Customer *cr[MAX_CR]; 

class TicketClerk : public Employee{
  private:
    // customer order ticket sum
    int ticketSum;
    // amount in one group
    double amount;
    // customer pay
    double payment;
  public:
    TicketClerk(int tcId);
    void sellTickets();
    void setTicketSum(int sum);
    int getTicketSum();
    // put amount in the variable
    void setAmount(int amount);
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
    void calAmount();
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
    void  checkSeated();
  public:
    MovieTechnician(int mtId);
    void WaitManager();
    void playMovie();
    void infoCustomer();
    void infoManager();
    
};
extern MovieTechnician * mt[MAX_MT];
class Manager : public Employee{
  public:
    void randToBreak(Lock * lockWaiting, Employee ** clerk, int count,  bool noClerk, Condition *cNoClerk, Semaphore * sNoClerk);
    // ask ticketTaker and customer to start
    void startTicketTaken();
    // ask MT to startMovie
    void startMovie();
    // collect money from clerk
    void collectMoney();
    void work();
    Manager(int mrId);
};
extern Manager * mr[MAX_MR];

void init();
#endif
