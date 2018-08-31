/* 
    File: semaphore.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/11

*/

#ifndef _semaphore_H_                   // include file only once
#define _semaphore_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>
#include <iostream>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   s e m a p h o r e  */
/*--------------------------------------------------------------------------*/
using namespace std; 
class semaphore {
private:
    /* -- INTERNAL DATA STRUCTURES */
    int capacity; 
    pthread_mutex_t m; 
    pthread_cond_t c; 

public:
    /* -- CONSTRUCTOR/DESTRUCTOR */
    semaphore(){}

    semaphore(int _val) {
        capacity = _val; 
        pthread_cond_init(&c, NULL);
        pthread_mutex_init(&m, NULL);
        //cout << capacity << "c"<<endl;          
	}

    ~semaphore(){
        pthread_cond_destroy(&c);
        pthread_mutex_destroy(&m);
    }
    /* -- SEMAPHORE OPERATIONS */
    //decrement/wait
    void P() {
        pthread_mutex_lock(&m); 
        capacity--; 
        if(capacity < 0){
           // cout << "waiting in p()"; 
            pthread_cond_wait(&c,&m);
            //cout << " DONE waiting in p()"; 

        }
        pthread_mutex_unlock(&m); 
        return; 
        
    }
    //increment/wake
    void V() {
        pthread_mutex_lock(&m); 
        capacity++; 
        if(capacity <= 0){            
            //cout << "signaling in v()"; 
            pthread_cond_signal(&c); 
            //cout << "done signaling in v()"; 
        }
        pthread_mutex_unlock(&m); 
    }
};

#endif


