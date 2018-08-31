/* 
    File: my_allocator.c

    Author: Sakshi Choudhary
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

/* Don't forget to implement "init_allocator" and "release_allocator"! */
Addr* freeList;
Addr header_ptr; 

int block_size, memory_size, num_blocks; 
//int header_size = sizeof(Header*);

unsigned int round_up(unsigned int n)
{
  unsigned count = 0;
 
  if (n && !(n&(n-1)))
    return n;
 
  while( n != 0)
  {
    n  >>= 1;
    count += 1;
  }
 
  return 1 << count;
}

void print_allocator (){
	Header* temp;
	printf("List: \n");
        for(int i = 0; i < num_blocks; i++){
            int index = memory_size/(pow(2,i));
	        temp = (Header*)freeList[i];
            int count = 0; 
            printf("%i : ",index);
	        while(temp){
                count ++; 
	    	    //printf("%i ",((Header*)temp)->size);
                //printf("block %i addr = %p, ", count,temp );
	    	    temp = ((Header*)temp)->next;
	    }
        printf("%i", count);
	    printf("\n");
	}
	printf("\n");
}

unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length){

   if(_basic_block_size < _length && (ceil(log2(_length)) == floor(log2(_length))) && (ceil(log2(_basic_block_size)) == floor(log2(_basic_block_size))) ){


        block_size = (_basic_block_size); 
        memory_size = (_length); 
        
        num_blocks = log2(memory_size/block_size)+1;

        //allocating memory
        //pointing to memory to freeList
        header_ptr = (char*) malloc(memory_size);
        
        freeList = (Addr*) malloc(num_blocks*sizeof(Header));

        //free list
        Header *head  = (Header*) header_ptr;
        //printf("header %p\n", head);
        head->size = memory_size;
        head->free = true;
        head->next = NULL;
        freeList[0] = head;

        for(int i = 1; i < num_blocks; i++){
          freeList[i] = NULL; 
        }  
        print_allocator(); 
        return memory_size; 
        //printf("header size: %i\n", sizeof(Header*));
    } 
    
}

int release_allocator(){
    free(header_ptr);
    free(freeList);
    return 0; 
}
    
extern Addr my_malloc(unsigned int _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
  int block = round_up(_length+sizeof(Header));
  //printf("block needed = %i\n", block);
  //printf("memory size = %i\n", memory_size );

  if(block < block_size){
      block = block_size;
  }
  if(block > memory_size){
      printf("block = %i,  memory_size = %i\n", block, memory_size);
      printf("block > memory_size");
      return NULL; 
      //exit(EXIT_FAILURE);
  }
  Header *h = NULL; 
  int i = 0;
  while(h==NULL && i < num_blocks){
        Header* temp = (Header*) freeList[i];
        if(temp != NULL && temp->size >= block){
            h = (Header*) freeList[i];
        }
        //free(temp);
        i++;
  }
  if(h == NULL){
      printf("Block not found");
      return NULL;
      //exit(EXIT_FAILURE);
  }
    while((h->size) > (block)){
      //printf("while split, hsize = %i\n", h->size);
      split_block(h);
      
  }
    //print_allocator();
    //printf("block size assigned = %i\n", h->size);
    delete_node(h); 
    h->free = false; 
   // print_allocator();
    //printf("addr =  %p , addr+1 = %p\n", h,(h+1));
   // printf("size of header = %i",sizeof(Header) );
    return (Addr) (h+1); 



 //return malloc((size_t)_length);
}

void split_block(Header *h){
    delete_node(h); 
    Header* new_h = (Header*) ((char*)h + h->size/2);
    h->size = (h->size/2);
    new_h->size = h->size;
    h->free = true; 
    new_h->free = true;

   // printf("split:block , h,new_h=  %i, %i \n", h->size, new_h->size);
    add(h); 
    add(new_h);
   // print_allocator(); 

}

void add(Header *node){
    int index = log2(memory_size/(node->size));
    //printf("add %i, %i\n newlist : \n", node->size,index); 

    node->next = (Header*) freeList[index];
    freeList[index] = node; 
}

void delete_node(Header *node){
    //printf("deleting node\n");
    int index = log2(memory_size/(node->size));
    //printf("index to delete = %i\n", index);
    //Header* head = (Header*) ((char*)node + node->size);
    if(freeList[index] != NULL){
        Header* head = (Header*) freeList[index]; 
        //printf("size head : %i\n", head->size);
        if(head == node){
            freeList[index] = node->next;
        }
        else{
            while(head->next != node){
                head = head->next; 
            }
            head->next = node->next;
    }
}
}


Header* merge_blocks(Header *node1, Header* node2){
   // printf("merging..");
    delete_node(node1); 
    delete_node(node2); 
    Header *head;
    
    if(node1 < node2){
        head = node1; 
    }
    else{
        head = node2; 
    }
    
    head->size = head->size*2;

    add(head); 
    return head; 

}

extern int my_free(Addr _a) {
    //printf("*****myfree\n");
    //printf("\nmy_free input  = %p\n", _a);
    Header* head = (Header*) ((char*)_a-sizeof(Header));

    //printf("address of header = %p\n", head);
    head->free  = true; 
    //printf("head size of my_free = %i\n", head->size); 
    add(head); 

    while(head->size < memory_size){
        Header *buddy = getBuddy(head); 
        //printf("buddy size = %i, buddy address = %p\n", buddy->size, buddy);
        if(buddy->free && buddy->size == head->size){
            head = merge_blocks(head,buddy);
        }
        else{
            return -1; 
        }
    }
   // print_allocator();
    //printf("done with my_free");
  
  return 0;
}

Header* getBuddy(Header* h){
    int index = log2(memory_size/(h->size));
    //printf("index in get buddy = %i\n", index); 
    //printf("getting buddy ...\n");
    //printf("freelist[index] = %p\n", freeList[index]);
    return (Header*) (((char*)h - (char*)header_ptr ^ (h->size)) + (char*)header_ptr);

}

