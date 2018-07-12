#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "ma_malloc.h"



#define MEM_POOL_SIZE 600 //in bytes
typedef unsigned char byte;

typedef enum {ALLOCATED, FREE} mem_status;

typedef struct {
  size size;
  mem_status status;
} mem_chunk_header;

static byte mem_pool[MEM_POOL_SIZE];


/*
** Allocates array of bytes (memory pool) and initializes the memory allocator. 
*/
void ma_init()
{
//printf("the size of struct is :%d/n",sizeof(mem_chunk_header));
byte *header = mem_pool;
((mem_chunk_header *)header)->size=MEM_POOL_SIZE-2*sizeof(mem_chunk_header);
((mem_chunk_header *)header)->status =FREE;
byte *footer;
footer = header+sizeof(mem_chunk_header)+((mem_chunk_header *)header)->size;  // make footer as same as header 
((mem_chunk_header *)footer)->size=((mem_chunk_header *)header)->size;
((mem_chunk_header *)footer)->status=FREE;

}

/*
** Requesting for the tsize bytes from memory pool. 
** If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
*/
void* ma_malloc(size tsize)
{
size remainsize = MEM_POOL_SIZE-2*sizeof(mem_chunk_header);
byte *ptr1;
byte *ptr1footer;
byte *ptr2;
ptr1 = mem_pool;		//point to the start
while(remainsize>tsize)     // when ptr1 is still in the pool
{
	if((((mem_chunk_header *)ptr1)->size)>tsize&&(((mem_chunk_header *)ptr1)->status)==FREE) // while size is enough and status is FREE
	{
	((mem_chunk_header *)ptr1)->size=tsize;		//add two headers in the memory
	((mem_chunk_header *)ptr1)->status =ALLOCATED;
	ptr1footer = ptr1+tsize+sizeof(mem_chunk_header);
	((mem_chunk_header *)ptr1footer) ->size=tsize;
	((mem_chunk_header *)ptr1footer) ->status=((mem_chunk_header *)ptr1)->status;

	ptr2 = ptr1+sizeof(mem_chunk_header);			//ptr2 is the return value

	remainsize=remainsize-tsize-2*sizeof(mem_chunk_header);
	ptr1=ptr1+((mem_chunk_header *)ptr1)->size+2*sizeof(mem_chunk_header);   //point to the next free block
	((mem_chunk_header *)ptr1)->size=remainsize;
	((mem_chunk_header *)ptr1)->status=FREE;

	ptr1footer = ptr1+remainsize+sizeof(mem_chunk_header);  // make the footer
	((mem_chunk_header *)ptr1footer) ->size=((mem_chunk_header *)ptr1)->size;
	((mem_chunk_header *)ptr1footer) ->status=((mem_chunk_header *)ptr1)->status;
	return (ptr2);
	}
	
	else{
	remainsize=remainsize-2*sizeof(mem_chunk_header)-((mem_chunk_header *)ptr1)->size;
	ptr1=ptr1+((mem_chunk_header *)ptr1)->size+2*sizeof(mem_chunk_header);
	}
}

return NULL;
}

/*
** Releasing the bytes in memory pool which was hold by ptr, meaning makes those bytes available for other uses. 
** Implement also the coalescing behavior.
*/
void ma_free(void* ptr)
{

	byte *temp;       //free this block
	temp=(byte *)ptr;
	temp=temp-sizeof(mem_chunk_header);
	((mem_chunk_header *)temp)->status = FREE;


	byte *next;
	byte *former;
	byte *foot;
	next=temp+2*sizeof(mem_chunk_header)+((mem_chunk_header *)temp)->size;		//free the next free block
	former=temp-sizeof(mem_chunk_header);			//find former block's foot
	foot=temp+((mem_chunk_header *)temp)->size+sizeof(mem_chunk_header);		//find this block's foot
	if(((mem_chunk_header *)next)->status == FREE)
	{
		((mem_chunk_header *)temp)->size=((mem_chunk_header *)temp)->size+2*sizeof(mem_chunk_header)+((mem_chunk_header *)next)->size;  //change the size if next is free
		foot=temp+sizeof(mem_chunk_header)+((mem_chunk_header *)temp)->size;		//add footer
		((mem_chunk_header *)foot)->status=((mem_chunk_header *)temp)->status;
		((mem_chunk_header *)foot)->size=((mem_chunk_header *)temp)->size;
	}
	if(((mem_chunk_header *)former)->status==FREE)
	{
		former=former+((mem_chunk_header *)former)->size+sizeof(mem_chunk_header);   //find the header of former block
		((mem_chunk_header *)former)->size=((mem_chunk_header *)former)->size+((mem_chunk_header *)temp)->size+16; //recalculate the free size
		((mem_chunk_header *)foot)->size=((mem_chunk_header *)former)->size;                 //copy the sizze to footer
	}

}


/*
** This function is only for debugging. It prints out the entire memory pool. 
** Use the code from the memdump tool to do this.
*/
void ma_print(void)
{
  
	//memdump( (byte *)mem_pool, MEM_POOL_SIZE );
	// use code from the memdump tool here to print the memory

  
}
 
  
