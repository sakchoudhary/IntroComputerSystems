/*
    File: client.cpp

    Author: J. Higginbotham
    Department of Computer Science
    Texas A&M University
    Date  : 2016/05/21

    Based on original code by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */
    /* -- This might be a good place to put the size of
        of the patient response buffers -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*
    No additional includes are required
    to complete the assignment, but you're welcome to use
    any that you think would help.
*/
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <assert.h>
#include <fstream>
#include <numeric>
#include <vector>
#include <iomanip>
#include <signal.h>
#include <fstream>


#include "reqchannel.h"
/*
    This next file will need to be written from scratch, along with
    semaphore.h and (if you choose) their corresponding .cpp files.
 */

#include "bounded_buffer.h"
using namespace std; 

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/*
    All *_params structs are optional,
    but they might help.
 */
struct PARAMS_request {
    bounded_buffer* bb_buffer; 
	string request; //request
    int n; 

};

struct PARAMS_WORKER {
    RequestChannel* channel; 
    
    bounded_buffer* b_buffer;
    bounded_buffer* jane_buffer;
    bounded_buffer* joe_buffer;
    bounded_buffer* john_buffer; 

};


struct PARAMS_STAT {

    string message;
  
    bounded_buffer* buffer;
    vector<int>* fqc; 
    int n; 


};

struct event{
    static vector<int>* jb1; 
    static vector<int>* jb2; 
    static vector<int>* jb3; 

    static void histogram_table(){
        std::stringstream tablebuilder;
        tablebuilder << std::setw(25) << std::right << "JOHN SMITH ";
        tablebuilder << std::setw(15) << std::right << "JANE SMITH";
        tablebuilder << std::setw(15) << std::right << "JOE SMITH" << std::endl;
        for (int i = 0; i < jb1->size(); ++i) {
            tablebuilder << std::setw(10) << std::left
                    << std::string(
                            std::to_string(i * 10) + "-"
                                    + std::to_string((i * 10) + 9));
            tablebuilder << std::setw(15) << std::right
                    << std::to_string(jb1->at(i));
            tablebuilder << std::setw(15) << std::right
                    << std::to_string(jb2->at(i));
            tablebuilder << std::setw(15) << std::right
                    << std::to_string(jb3->at(i)) << std::endl;
        
        tablebuilder << std::setw(10) << std::left << "Total";
        tablebuilder << std::setw(15) << std::right
                << accumulate(jb1->begin(), jb1->end(), 0);
        tablebuilder << std::setw(15) << std::right
                << accumulate(jb2->begin(), jb2->end(), 0);
        tablebuilder << std::setw(15) << std::right
                << accumulate(jb3->begin(), jb3->end(), 0) << std::endl;
        }
        cout << tablebuilder.str();
    }

    static void event_handler(int n){
        system("clear");
        histogram_table(); 
       signal(SIGALRM, event_handler);
       alarm(2);
    }
};
vector<int>* event::jb1; 
vector<int>* event::jb2; 
vector<int>* event::jb3;



/*
    This class can be used to write to standard output
    in a multithreaded environment. It's primary purpose
    is printing debug messages while multiple threads
    are in execution.
 */
class atomic_standard_output {
    pthread_mutex_t console_lock;
public:
    atomic_standard_output() { pthread_mutex_init(&console_lock, NULL); }
    ~atomic_standard_output() { pthread_mutex_destroy(&console_lock); }
    void print(std::string s){
        pthread_mutex_lock(&console_lock);
        std::cout << s << std::endl;
        pthread_mutex_unlock(&console_lock);
    }
};

atomic_standard_output threadsafe_standard_output;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* HELPER FUNCTIONS */
/*--------------------------------------------------------------------------*/

std::string make_histogram_table(std::string name1, std::string name2,
    std::string name3, std::vector<int> *data1, std::vector<int> *data2,
    std::vector<int> *data3) {
	std::stringstream tablebuilder;
	tablebuilder << std::setw(25) << std::right << name1;
	tablebuilder << std::setw(15) << std::right << name2;
	tablebuilder << std::setw(15) << std::right << name3 << std::endl;
	for (int i = 0; i < data1->size(); ++i) {
		tablebuilder << std::setw(10) << std::left
		        << std::string(
		                std::to_string(i * 10) + "-"
		                        + std::to_string((i * 10) + 9));
		tablebuilder << std::setw(15) << std::right
		        << std::to_string(data1->at(i));
		tablebuilder << std::setw(15) << std::right
		        << std::to_string(data2->at(i));
		tablebuilder << std::setw(15) << std::right
		        << std::to_string(data3->at(i)) << std::endl;
	}
	tablebuilder << std::setw(10) << std::left << "Total";
	tablebuilder << std::setw(15) << std::right
	        << accumulate(data1->begin(), data1->end(), 0);
	tablebuilder << std::setw(15) << std::right
	        << accumulate(data2->begin(), data2->end(), 0);
	tablebuilder << std::setw(15) << std::right
	        << accumulate(data3->begin(), data3->end(), 0) << std::endl;

	return tablebuilder.str();
}

/*
    You'll need to fill these in.
*/
void* request_thread_function(void* arg) {
	PARAMS_request rq = *(PARAMS_request*) arg;

	for(int i = 0; i < rq.n; i++) {
		rq.bb_buffer->push_back(rq.request); 
        
	}
    // cout << "\nbuffer size = " << rq.bb_buffer->size() <<endl; 
}

void* worker_thread_function(void* arg) {
  	PARAMS_WORKER wkr = *(PARAMS_WORKER*) arg;

    RequestChannel *rchannel = wkr.channel;
	string s1 = rchannel->send_request("newthread");
	RequestChannel *workerChannel =  new RequestChannel(s1, RequestChannel::CLIENT_SIDE);
    while(true) {
		string request = wkr.b_buffer->retrieve_front(); 
		string message = workerChannel->send_request(request);

		if(request == "data John Smith") {
			wkr.john_buffer->push_back(message);
        }
		else if(request == "data Jane Smith") {
			wkr.jane_buffer->push_back(message);
		}
		else if(request == "data Joe Smith") {
			wkr.joe_buffer->push_back(message);
		}
		else if(request == "quit") {
			delete workerChannel;
			break;
		}

    }

}

void* stat_thread_function(void* arg) {
	
    PARAMS_STAT stat = *(PARAMS_STAT*) arg;
    string message; 
    int n = stat.n;
    //cout << "n = " << n << endl; 
    int val = 0; 

    while(val < n){
       //cout << "val = " << val <<endl; 
       //cout << "stat size = " << stat.buffer->size() <<endl; 
       message = stat.buffer->retrieve_front(); 
       //cout << "message = " << message.c_str() <<endl; 
       stat.fqc->at(stoi(message) / 10) += 1;
       val ++; 

    }
    

}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
    int n = 10; //default number of requests per "patient"
    int b = 50; //default size of request_buffer
    int w = 10; //default number of worker threads
    bool USE_ALTERNATE_FILE_OUTPUT = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:b:w:m:h")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg);
                break;
            case 'm':
                if(atoi(optarg) == 2) USE_ALTERNATE_FILE_OUTPUT = true;
                break;
            case 'h':
            default:
                std::cout << "This program can be invoked with the following flags:" << std::endl;
                std::cout << "-n [int]: number of requests per patient" << std::endl;
                std::cout << "-b [int]: size of request buffer" << std::endl;
                std::cout << "-w [int]: number of worker threads" << std::endl;
                std::cout << "-m 2: use output2.txt instead of output.txt for all file output" << std::endl;
                std::cout << "-h: print this message and quit" << std::endl;
                std::cout << "Example: ./client_solution -n 10000 -b 50 -w 120 -m 2" << std::endl;
                std::cout << "If a given flag is not used, a default value will be given" << std::endl;
                std::cout << "to its corresponding variable. If an illegal option is detected," << std::endl;
                std::cout << "behavior is the same as using the -h flag." << std::endl;
                exit(0);
        }
    }

    std::vector<int> john_frequency_count(10, 0);
    std::vector<int> jane_frequency_count(10, 0);
    std::vector<int> joe_frequency_count(10, 0);

    bounded_buffer bb_buffer  =  bounded_buffer(b);
    bounded_buffer bb_jane = bounded_buffer(b); 
    bounded_buffer bb_john  = bounded_buffer(b); 
    bounded_buffer bb_joe  = bounded_buffer(b); 


    int pid = fork();
	if (pid > 0) {
        struct timeval start_time;
        struct timeval finish_time;
        int64_t start_usecs;
        int64_t finish_usecs;
        std::ofstream ofs;
        if(USE_ALTERNATE_FILE_OUTPUT) ofs.open("output2.txt", std::ios::out | std::ios::app);
        else ofs.open("output.txt", std::ios::out | std::ios::app);

        std::cout << "n == " << n << std::endl;
        std::cout << "b == " << b << std::endl;
        std::cout << "w == " << w << std::endl;

        std::cout << "CLIENT STARTED:" << std::endl;
        std::cout << "Establishing control channel... " << std::flush;
        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
        std::cout << "done." << std::endl;

        /*
            This time you're up a creek.
            What goes in this section of the code?
            Hint: it looks a bit like what went here
            in PA3, but only a *little* bit.
         */
    
    /*
        CREATE WORKER THREADS
    */
	    std::cout << "creating worker threads... ";
		//fflush(NULL);
        pthread_t worker_thread[w]; 

		PARAMS_WORKER w1; 
		w1.channel = chan; 
		w1.b_buffer = &bb_buffer; 
		w1.joe_buffer = &bb_joe; 
		w1.jane_buffer = &bb_jane; 
		w1.john_buffer = &bb_john;


		for(int i = 0; i < w; i++){    
			pthread_create(&worker_thread[i], NULL, worker_thread_function, &w1);
		}

        cout << "done" <<endl; 
        event e1; 
        e1.jb1 = &john_frequency_count; 
        e1.jb2 = &jane_frequency_count; 
        e1.jb3 = &joe_frequency_count;

    /*
        CREATE STAT THREADS
    */
        pthread_t stat_john; 
        pthread_t stat_jane; 
        pthread_t stat_joe;
       
       // fflush(NULL);

        PARAMS_STAT sjohn; 
        sjohn.n = n; 
        sjohn.buffer = &bb_john;  
        sjohn.fqc = &john_frequency_count;


        PARAMS_STAT sjane; 
        sjane.n = n; 
        sjane.buffer = &bb_jane;  
        sjane.fqc = &jane_frequency_count;

        PARAMS_STAT sjoe; 
        sjoe.n = n; 
        sjoe.buffer = &bb_joe;   
        sjoe.fqc = &joe_frequency_count;
	   
        std::cout << "creating stat threads... ";

        pthread_create(&stat_john, NULL, stat_thread_function, &sjohn);
        pthread_create(&stat_jane, NULL, stat_thread_function, &sjane);
        pthread_create(&stat_joe, NULL, stat_thread_function, &sjoe);
        cout << "done" <<endl; 
        

		/*-------------------------------------------*/
		/* START TIMER HERE */
		/*-------------------------------------------*/

		struct timeval tp_start; /* Used to compute elapsed time. */
  		struct timeval tp_end;
    	
		assert(gettimeofday(&tp_start, 0) == 0);

		clock_t start; 
		float duration; 
		start = clock(); 
        signal(SIGALRM, event::event_handler);
        alarm(2);

    /*
        CREATE REQUEST THREADS
    */
        pthread_t john; 
		pthread_t jane; 
		pthread_t joe; 
		
		fflush(NULL);

		PARAMS_request t1; 
		t1.bb_buffer = &bb_buffer; 
		t1.request = "data John Smith"; 
		t1.n = n;

		PARAMS_request t2; 
		t2.bb_buffer = &bb_buffer;
		t2.request = "data Jane Smith"; 
		t2.n = n;

		PARAMS_request t3; 
		t3.bb_buffer = &bb_buffer;
		t3.request = "data Joe Smith";
		t3.n = n;



        std::cout << "creating request threads... ";

		pthread_create(&john, NULL, request_thread_function, &t1);
		pthread_create(&jane, NULL, request_thread_function, &t2); 
		pthread_create(&joe, NULL, request_thread_function, &t3); 
        std::cout << "done "<<endl;

    
    /*
        JOIN REQUEST THREADS
    */        
        cout << "waiting for request threads to join... "; 
		pthread_join(john,NULL);
		pthread_join(jane,NULL);
		pthread_join(joe,NULL);
        cout << "done" <<endl;


    /*
        PUSH QUITS
    */

        cout << "pushing quits...  "; 
        fflush(NULL);
        for(int i = 0; i < w; ++i) {
            bb_buffer.push_back("quit");
        }
        cout << "done" <<endl;

    /*
        JOIN WORKER THREADS
    */
        cout << "waiting for worker threads  to join... "; 
        for(int i = 0; i < w; i ++){
			pthread_join(worker_thread[i],NULL);
		} 
        cout << "done" <<endl; 

    /*
        JOIN STAT THREADS
    */
        cout << "waiting for stat to join... "; 
		pthread_join(stat_john,NULL);
		pthread_join(stat_jane,NULL);
		pthread_join(stat_joe,NULL);
        cout << "done" <<endl; 


        /*-------------------------------------------*/
        /* END TIMER HERE   */
        /*-------------------------------------------*/
 		duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
		assert(gettimeofday(&tp_end, 0) == 0);

		long sec = tp_end.tv_sec - tp_start.tv_sec;
		long musec = tp_end.tv_usec - tp_start.tv_usec;
		if (musec < 0) {
			musec += 1000000;
			sec--;
		}
		cout << "[sec = "<< sec << ", musec = "<< musec << "]"<<endl;


		fstream ff;
		ff.open("output.csv",fstream::app);
		if(ff.is_open()){
			ff << b << "," << sec <<"," << musec << endl;
			std::cout << "Finished!" << std::endl;
			ff.close(); 
		}

    /* 
        HISTOGRMS
    */

    std::string histogram_table = make_histogram_table("John Smith",
            "Jane Smith", "Joe Smith", &john_frequency_count,
            &jane_frequency_count, &joe_frequency_count);

    std::cout << "Results for n == " << n << ", w == " << w << std::endl;

    /*
        This is a good place to output your timing data.
    */

    std::cout << histogram_table << std::endl;

    //signal(SIGALRM,signal_handler); 

        ofs.close();
        std::cout << "Sleeping..." << std::endl;
        usleep(10000);
        std::string finale = chan->send_request("quit");
        std::cout << "Finale: " << finale << std::endl;
    }
	else if (pid == 0)
		execl("dataserver", NULL);
}
