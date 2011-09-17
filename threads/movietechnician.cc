// MovieTechnician
#include "movietheater.h"

MovieTechnician::MovieTechnician(int id) : Employee(id, "MovieTechnician"){


}

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

void MovieTechnician::infoCustomer(){
     
    int i=0;

    for(i=0;i< MAX_SEAT;i++){
        if(seatState[i]==true){
            sSeat[i]->V();
        }
    }
    printf("The MovieTechnician has told all customers to leave\n");

}
