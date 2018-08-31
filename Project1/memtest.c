#include "ackerman.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include "my_allocator.h"

//Sakshi Choudhary 

int main(int argc, char ** argv) {


  int basic_block_size = 128;
  int memory_length =524288;
  int opt; 
  while((opt = getopt(argc, argv, "b:s:")) != -1){
    switch(opt){
      case 'b':
        basic_block_size = atoi(optarg); 
        break; 
      case 's':
        memory_length = atoi(optarg);
        break;
    }
  }

  
  basic_block_size = round_up(basic_block_size); 
  memory_length = round_up(memory_length); 
  //printf("basic_block_size = %i, memory_size = %i\n", basic_block_size, memory_length);

  init_allocator(basic_block_size, memory_length);
  /*print_allocator(); 
	Addr a = my_malloc(100);
  printf("\nmy_mallc = %p\n", a); 
  print_allocator(); 
  my_free(a);
  print_allocator(); 

*/
  ackerman_main(); // this is the full-fledged test. 
 // print_allocator(); 
  // The result of this function can be found from the ackerman wiki page or https://www.wolframalpha.com/. If you are not getting correct results, that means that your allocator is not working correctly. In addition, the results should be repetable - running ackerman (3, 5) should always give you the same correct result. If it does not, there must be some memory leakage in the allocator that needs fixing
  
  // please make sure to run small test cases first before unleashing ackerman. One example would be running the following: "print_allocator (); x = my_malloc (1); my_free(x); print_allocator();" the first and the last print should be identical.
  
  release_allocator();
}
