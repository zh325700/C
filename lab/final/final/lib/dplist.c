#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * definition of error codes
 * */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list 

#ifdef DEBUG
	#define DEBUG_PRINTF(...) 									         \
		do {											         \
			fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	 \
			fprintf(stderr,__VA_ARGS__);								 \
			fflush(stderr);                                                                          \
                } while(0)
#else
	#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition,err_code)\
	do {						            \
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");    \
            assert(!(condition));                                    \
        } while(0)

        
/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
  dplist_node_t * prev, * next;
  void * element;
};

struct dplist {
  dplist_node_t * head;
  void * (*element_copy)(void * src_element);			  
  void (*element_free)(void ** element);
  int (*element_compare)(void * x, void * y);
};


dplist_t * dpl_create (// callback functions
			  void * (*element_copy)(void * src_element),
			  void (*element_free)(void ** element),
			  int (*element_compare)(void * x, void * y)
			  )
{
  dplist_t * list;
  list = malloc(sizeof(struct dplist));
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_MEMORY_ERROR);
  list->head = NULL;  
  list->element_copy = element_copy;
  list->element_free = element_free;
  list->element_compare = element_compare; 
  return list;
}

void dpl_free(dplist_t ** list, bool free_element)
{
    // add your code here
}

dplist_t * dpl_insert_at_index(dplist_t * list, void * element, int index, bool insert_copy)
{
    // add your code here
}

dplist_t * dpl_remove_at_index( dplist_t * list, int index, bool free_element)
{
    // add your code here
}

int dpl_size( dplist_t * list )
{
    // add your code here
}

dplist_node_t * dpl_get_reference_at_index( dplist_t * list, int index )
{
    // add your code here
}

void * dpl_get_element_at_index( dplist_t * list, int index )
{
    // add your code here
}

int dpl_get_index_of_element( dplist_t * list, void * element )
{
    // add your code here
}

// HERE STARTS THE EXTRA SET OF OPERATORS //

// ---- list navigation operators ----//
  
dplist_node_t * dpl_get_first_reference( dplist_t * list )
{
    // add your code here
}

dplist_node_t * dpl_get_last_reference( dplist_t * list )
{
    // add your code here
}

dplist_node_t * dpl_get_next_reference( dplist_t * list, dplist_node_t * reference )
{
    // add your code here
}

dplist_node_t * dpl_get_previous_reference( dplist_t * list, dplist_node_t * reference )
{
    // add your code here
}

// ---- search & find operators ----//  
  
void * dpl_get_element_at_reference( dplist_t * list, dplist_node_t * reference )
{
    // add your code here
}

dplist_node_t * dpl_get_reference_of_element( dplist_t * list, void * element )
{
    // add your code here
}

int dpl_get_index_of_reference( dplist_t * list, dplist_node_t * reference )
{
    // add your code here
}
  
// ---- extra insert & remove operators ----//

dplist_t * dpl_insert_at_reference( dplist_t * list, void * element, dplist_node_t * reference, bool insert_copy )
{
    // add your code here
}

dplist_t * dpl_insert_sorted( dplist_t * list, void * element, bool insert_copy )
{
    // add your code here
}

dplist_t * dpl_remove_at_reference( dplist_t * list, dplist_node_t * reference, bool free_element )
{
    // add your code here
}

dplist_t * dpl_remove_element( dplist_t * list, void * element, bool free_element )
{
    // add your code here
}
  
// ---- you can add your extra operators here ----//



