//
//  bounded_buffer.hpp
//  
//
//  Created by Joshua Higginbotham on 11/4/15.
// Sakshi Choudhary
//

#ifndef bounded_buffer_h
#define bounded_buffer_h

#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>
#include "semaphore.h"

using namespace std; 

class bounded_buffer {
	/* Internal data here */
    semaphore *empty; 
    semaphore *full; 
    semaphore *mutex; 
    queue<string> data; 
public:
    bounded_buffer(); 
    bounded_buffer(int _capacity);
    ~bounded_buffer(); 
    void push_back(string str);
    string retrieve_front();
    int size();
};

#endif /* bounded_buffer_h */
