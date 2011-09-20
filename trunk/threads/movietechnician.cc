// MovieTechnician
#include "movietheater.h"

MovieTechnician::MovieTechnician(int id) : Employee(id, "MovieTechnician"){
    movieState = 1;

}
void MovieTechnician::WaitManager(){
    DEBUGINFO('c', "%s WaitManager", getEmployeeType() );
    sStartMovie->P();
    DEBUGINFO('c', "%s checkSeated", getEmployeeType() );
    checkSeated();
}

void MovieTechnician::checkSeated(){
    int i=0;
    DEBUGINFO('c', "%s acquire lTicketTaken, lTicketTaken's owner : %s", getEmployeeType(), lTicketTaken->getOwnerThread() == NULL? "NULL": lTicketTaken->getOwnerThread()->getName());
    
    lTicketTaken->Acquire();
    DEBUGINFO('c', "%s acquire lStartMovie, lStartMovie's owner : %s", getEmployeeType(), lStartMovie->getOwnerThread() == NULL? "NULL": lStartMovie->getOwnerThread()->getName());
    lStartMovie->Acquire();    

DEBUGINFO('c', "tickettaken %d\n", ticketTaken);

    for(i=0;i<ticketTaken;i++){  
          
        sMT_CR_Check->V();
        cMT_CR_Check->Wait(lStartMovie);
    }
    lStartMovie->Release();  
    lTicketTaken->Release();     
    playMovie();
}
void MovieTechnician::playMovie(){

    int i;
    int period = (rand()% MOVIE_PERIOD_VAR) + MOVIE_PERIOD_BASE;

    //bIsMovieOver=false;
    // start movie    
    printf("The MovieTechnician has started the movie.\n");
    for (i = 0; i < period; i++) {
        currentThread->Yield();
    }
    printf("The MovieTechnician has ended the movie.\n"); 
    infoCustomer();
}

void MovieTechnician::infoCustomer(){
     
    int i=0;

    //TODO: 
    lStopMovie->Acquire();
    for(i=0;i< MAX_SEAT;i++){
        DEBUGINFO('c', "seatState[%d]: %d", i, seatState[i]?1:0);
        if(seatState[i]==true){
            DEBUGINFO('c', "%s sMT_CR_Stop->V() to seat[%d]", getEmployeeType(), i);
            sMT_CR_Stop->V();
            cMT_CR_Stop->Wait(lStopMovie);
            seatState[i]=false; //reset seat state
        }
    }
    lStopMovie->Release();
    printf("The MovieTechnician has told all customers to leave the theater room.\n"); 

    infoManager();
    
}
void  MovieTechnician::infoManager(){
    //info manager
    DEBUGINFO('c', "%s infoManager", getEmployeeType() );
    lMovieState->Acquire();
    movieState=0;    
    lMovieState->Release();
    DEBUGINFO('c', "%s finish infoManager", getEmployeeType());
    WaitManager();

}


