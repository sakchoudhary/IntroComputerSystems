//
//  SafeBuffer.cpp
//
//
//  Created by Joshua Higginbotham on 11/4/15.
//
//

#include "SafeBuffer.h"
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <pthread.h>

using namespace std; 

SafeBuffer::SafeBuffer() {
	pthread_mutex_init (&mtx, NULL); 
}

SafeBuffer::~SafeBuffer() {
	pthread_mutex_destroy(&mtx); 
}

int SafeBuffer::size() {
    return this->requests.size();
}

void SafeBuffer::push_back(std::string str) {
	
	pthread_mutex_lock(&mtx); 
	requests.push(str) ;
	pthread_mutex_unlock(&mtx); 

	return;
}
void SafeBuffer::initialize() {
	pthread_mutex_init (&mtx, NULL); 
}
std::string SafeBuffer::retrieve_front() {
	pthread_mutex_lock(&mtx); 
	string front = requests.front(); 
	requests.pop();
	pthread_mutex_unlock(&mtx); 
  
	return front;
}
