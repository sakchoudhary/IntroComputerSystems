/* 
    File: dataserver.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/16

    Dataserver main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "NetworkRequestChannel.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* VARIABLES */
pthread_mutex_t channel_mutex;
/*--------------------------------------------------------------------------*/

static int nthreads = 0;
static int port, backlog; 

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void* handle_process_loop(void * arg);

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

	/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/

/*void * handle_data_requests(void * args) {

	NetworkRequestChannel * data_channel =  (NetworkRequestChannel*) args;

	// -- Handle client requests on this channel. 

	handle_process_loop(*data_channel);

	// -- Client has quit. We remove channel.

	delete data_channel;
	
	return nullptr;
}*/

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(NetworkRequestChannel * _channel, const string & _request) {
	_channel->cwrite("hello to you too");
}

void process_data(NetworkRequestChannel * _channel, const string &  _request) {
	usleep(1000 + (rand() % 5000));
	//_channel.cwrite("here comes data about " + _request.substr(4) + ": " + std::to_string(random() % 100));
	_channel->cwrite(std::to_string(rand() % 100));
}

/*void process_newthread(NetworkRequestChannel & _channel, const string & _request) {
	pthread_mutex_lock(&channel_mutex);	
	int error;
	nthreads ++;

	// -- Name new data channel

	std::string new_channel_name = "data" + std::to_string(nthreads) + "_";
	//  std::cout << "new channel name = " << new_channel_name << endl;

	// -- Pass new channel name back to client
	_channel.cwrite(new_channel_name);
	pthread_mutex_unlock(&channel_mutex);
 
  	// -- Construct new data channel (pointer to be passed to thread function)
	try {
		NetworkRequestChannel * data_channel = new NetworkRequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);

		// -- Create new thread to handle request channel

		pthread_t thread_id;
		//  std::cout << "starting new thread " << nthreads << endl;
		if ((errno = pthread_create(& thread_id, NULL, handle_data_requests, data_channel)) != 0) {
			perror(std::string("DATASERVER: " + _channel.name() + ": pthread_create failure").c_str());
			delete data_channel;
		}
	}
	catch (sync_lib_exception sle) {
		perror(std::string(sle.what()).c_str());
	}

}*/

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(NetworkRequestChannel * _channel, const string & _request) {

	if (_request.compare(0, 5, "hello") == 0) {
		process_hello(_channel, _request);
	}
	else if (_request.compare(0, 4, "data") == 0) {
		process_data(_channel, _request);
	}
	// else if (_request.compare(0, 9, "newthread") == 0) {
	// 	process_newthread(_channel, _request);
	// }
	else {
		_channel->cwrite("unknown request");
	}
}

void *handle_process_loop(void * arg) {
	NetworkRequestChannel * _channel = (NetworkRequestChannel *) arg;
	for(;;) {
		//cout << "Reading next request from channel \n"<< flush;

		std::string request = _channel->cread();
		//cout << " done (" << ")." << endl;
    	//cout << "New request is " << request << endl;

		process_request(_channel, request);
	}
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/


int main(int argc, char * argv[]) {

	int opt; 
	port = 2019; 
	backlog = 50;
	while ((opt = getopt(argc, argv, "p:b:")) != -1) {
        switch (opt) {
            case 'b':
                backlog = atoi(optarg);
                break;
            case 'p':
                port= atoi(optarg);
                break;

        }
    }

	//  std::cout << "Establishing control channel... " << std::flush;
	//pthread_mutex_init (&channel_mutex, NULL);
	NetworkRequestChannel control_channel(port,handle_process_loop, backlog);
	//  std::cout << "done.\n" << std::flush;

	//handle_process_loop(control_channel);
}

