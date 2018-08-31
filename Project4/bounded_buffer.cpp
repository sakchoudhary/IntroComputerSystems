//
//  bounded_buffer.cpp
//  
//
//  Created by Joshua Higginbotham on 11/4/15.
// Sakshi Choudhary
//

#include "bounded_buffer.h"
#include "semaphore.h"
#include <string>
#include <queue>
#include<iostream>
using namespace std; 

bounded_buffer::bounded_buffer(){}

bounded_buffer::bounded_buffer(int _capacity){
   // cout << "capacity" << _capacity <<endl; 
    empty = new semaphore(_capacity); 
    full =  new semaphore(0) ;
    mutex = new semaphore(1);  
}
bounded_buffer::~bounded_buffer(){
    delete empty; 
    delete full;
    delete mutex; 
}

void bounded_buffer::push_back(string req) {
   // cout << "push_back , bounded buffer" <<endl; 
    empty->P(); 
    mutex->P(); 
    data.push(req); 
    mutex->V(); 
    full->V(); 
}

string bounded_buffer::retrieve_front() {
    //cout << "retrive_front , bounded buffer" <<endl; 

    full->P(); 
    mutex->P(); 
    string item = data.front(); 
    data.pop();
    mutex->V(); 
    empty->V(); 
    return item;  

}

int bounded_buffer::size() {
    return data.size(); 
}