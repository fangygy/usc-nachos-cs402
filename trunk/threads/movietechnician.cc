// MovieTechnician
#include "movietheater.h"

MovieTechnician::MovieTechnician(int id) : Employee(id, "MovieTechnician"){
    movieState = 1;

}
//movie technician wait for manager to tell him to start a movie
void MovieTechnician::WaitManager(){
    DEBUGINFO('c', "%s WaitManager", getEmployeeType() );
    sStartMovie->P();
    DEBUGINFO('c', "%s checkSeated", getEmployeeType() );
    //if movie technician has been told to start a movie, check if all customer seated.
    checkSeated();
}
//movie technician check if all customer seated.
void MovieTechnician::checkSeated(){
    int i=0;
    DEBUGINFO('c', "%s acquire lTicketTaken, lTicketTaken's owner : %s", getEmployeeType(), lTicketTaken->getOwnerThread() == NULL? "NULL": lTicketTaken->getOwnerThread()->getName());
    
    lTicketTaken->Acquire();
    DEBUGINFO('c', "%s acquire lStartMovie, lStartMovie's owner : %s", getEmployeeType(), lStartMovie->getOwnerThread() == NULL? "NULL": lStartMovie->getOwnerThread()->getName());
    
    lStartMovie->Acquire();    

DEBUGINFO('c', "tickettaken %d\n", ticketTaken);
   
    //wake up the customer according to the number of tickets taken
    for(i=0;i<ticketTaken;i++){      
        sMT_CR_Check->V();
        cMT_CR_Check->Wait(lStartMovie);
    }
    lStartMovie->Release();  
    lTicketTaken->Release();    

    //movie technician has checked all customers and start movie 
    playMovie();
}
//movie techinician play movie 
void MovieTechnician::playMovie(){

    int i;
    int period = (rand()% MOVIE_PERIOD_VAR) + MOVIE_PERIOD_BASE;

    // start movie    
    printf("The MovieTechnician has started the movie.\n");
    for (i = 0; i < period; i++) {
        currentThread->Yield();
    }
    printf("The MovieTechnician has ended the movie.\n"); 
    infoCustomer();
}
//movie techinician wake up all customer after movies is over
void MovieTechnician::infoCustomer(){
     
    int i=0;

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

    //movie techinician tell manager movies is over
    infoManager();
    
}
//movie techinician tell manager movies is over
void  MovieTechnician::infoManager(){
    //info manager
    DEBUGINFO('c', "%s infoManager", getEmployeeType() );
    lMovieState->Acquire();
    movieState=0;    
    lMovieState->Release();
    DEBUGINFO('c', "%s finish infoManager", getEmployeeType());
    WaitManager();

}


