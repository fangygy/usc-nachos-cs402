// MovieTechnician
#include "movietheater.h"

MovieTechnician::MovieTechnician(int id) : Employee(id, "MovieTechnician"){


}
void MovieTechnician::WaitManager(){
    
    printf("DEBUG:MT wait manager to start movie\n");
    sStartMovie->P();
    printf("DEBUG:MT has been told to start movie\n");

    checkSeated();

}

void MovieTechnician::checkSeated(){
    int i=0;
    lStartMovie->Acquire();    
    for(i=0;i<ticketTaken;i++){  
          
        sMT_CR_Check->V();
        cMT_CR_Check->Wait(lStartMovie);
    }
    lStartMovie->Release();       
    printf("DEBUG:The MovieTechnician has check all customer\n");
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
        if(seatState[i]==true){
            
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
    lIsMovieOver->Acquire();
    bIsMovieOver=true;    
    lIsMovieOver->Release();

    WaitManager();
     
  

}

