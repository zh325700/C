#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

//#define DEBUG

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
  element_t element;
};

struct dplist {
  dplist_node_t * head;
  // more fields will be added later
};

dplist_t * dpl_create ()
{
  dplist_t * list;
  list = malloc(sizeof(struct dplist));
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_MEMORY_ERROR);
  list->head = NULL;  
  // pointer drawing breakpoint
  return list;
}

									// FREE

void dpl_free( dplist_t ** list )
{
  // add your code here
int size = dpl_size(*list);
int count = size-1;
dplist_node_t * ref_at_index;
while(count>=0)
{
ref_at_index = dpl_get_reference_at_index(*list, count);
free(ref_at_index);
count--;
}
free(*list);
}

  /* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
   * 1. empty list ==> avoid errors
   * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
   * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
   * 4. do operation in the middle of the list ==> default case with default pointer manipulation
   * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
   **/ 

dplist_t * dpl_insert_at_index( dplist_t * list, element_t element, int index)
{
  dplist_node_t * ref_at_index, * list_node;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  list_node = malloc(sizeof(dplist_node_t));
  DPLIST_ERR_HANDLER(list_node==NULL,DPLIST_MEMORY_ERROR);
  list_node->element = element;
  // pointer drawing breakpoint
  if (list->head == NULL)  
  { // covers case 1 
    list_node->prev = NULL;
    list_node->next = NULL;
    list->head = list_node;
    // pointer drawing breakpoint
  } else if (index <= 0)  
  { // covers case 2 
    list_node->prev = NULL;
    list_node->next = list->head;
    list->head->prev = list_node;
    list->head = list_node;
    // pointer drawing breakpoint
  } else 
  {
    ref_at_index = dpl_get_reference_at_index(list, index);  
    assert( ref_at_index != NULL);
    // pointer drawing breakpoint
    if (index < dpl_size(list))                        //index 在 List 的里面，就加在list中间
    { // covers case 4
      list_node->prev = ref_at_index->prev;
      list_node->next = ref_at_index;
      ref_at_index->prev->next = list_node;
      ref_at_index->prev = list_node;
      // pointer drawing breakpoint
    } else							//index 超出了list的大小，就加在最后米那
    { // covers case 3 
      assert(ref_at_index->next == NULL);
      list_node->next = NULL;
      list_node->prev = ref_at_index;
      ref_at_index->next = list_node;    
      // pointer drawing breakpoint
    }
  }
  return list;
}

								//dpl_remove_at_index

dplist_t * dpl_remove_at_index( dplist_t * list, int index)
{
  // add your code here
  dplist_node_t * ref_at_index;
  dplist_node_t * temp;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
    if (list->head == NULL)  
  { // covers case 1 
  printf("the list is empty");
  return list;
  } else if (index <= 0)  
  { // covers case 2 
	temp=list->head;
	list->head=temp->next;
	temp->next->prev = NULL;
	free(temp);                             //free the first node
  } else 
  {
    ref_at_index = dpl_get_reference_at_index(list, index);  
    assert( ref_at_index != NULL);
    if (index < dpl_size(list))                        //index 在 List 的里面，就减去中间
    { // covers case 4
	ref_at_index = dpl_get_reference_at_index(list, index); 
	ref_at_index->prev->next = ref_at_index->next;
	ref_at_index->next->prev = ref_at_index->prev;
	free(ref_at_index);
    } else							//index 超出了list的大小，就减去最后面辣个
    { // covers case 3 
   ref_at_index = dpl_get_reference_at_index(list, index);
   ref_at_index->prev->next = NULL;
   free(ref_at_index);
    }
  }
  return list;

}

									//dpl_size

int dpl_size( dplist_t * list )
{
  // add your code here
int count=0;
dplist_node_t * dummy;
DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
if (list->head == NULL ) return 0;
dummy=list->head;
while(dummy->next!=NULL) 
{
dummy=dummy->next;
count++;
}
return (count+1);
}

								//dpl_get_reference_at_index

dplist_node_t * dpl_get_reference_at_index( dplist_t * list, int index )
{
  int count;
  dplist_node_t * dummy;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  if (list->head == NULL ) return NULL;
  for ( dummy = list->head, count = 0; dummy->next != NULL ; dummy = dummy->next, count++) 
  { 
    if (count >= index) return dummy;
  }  
  return dummy;  
}

								//dpl_get_element_at_index

element_t dpl_get_element_at_index( dplist_t * list, int index )       //
{
  // add your code here
  int count;
  dplist_node_t * dummy;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  if (list->head == NULL ) return 0;
  for ( dummy = list->head, count = 0; dummy->next != NULL ; dummy = dummy->next, count++) 
  { 
    if (count >= index) return dummy->element;
  }  
  return dummy->element; 
}

								//dpl_get_index_of_element

int dpl_get_index_of_element( dplist_t * list, element_t element )
{
  // add your code here
    int count;
  dplist_node_t * dummy;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  if (list->head == NULL ) return -1;
  for ( dummy = list->head, count = 0; dummy->next != NULL ; dummy = dummy->next, count++) 
  { 
    if (dummy->element == element) return count;
  }  
  if(dummy->element == element)                 // if the element in case equals to the last node's value.(but the last node will not be check in for loop)  
  {
  return count;
  }else
  {
  return -1;
  }
  return count;
}



