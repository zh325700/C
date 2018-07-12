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
			  void * (*element_copy)(void * src_element),      //返回值是一个指针,返回一个deep copy以后的指针，把这个指针加入到list中
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
								//dpl_free function(need callback function)
void dpl_free(dplist_t ** list, bool free_element)
{
    // add your code here
int size = dpl_size(*list);
int i;
DPLIST_ERR_HANDLER(list==NULL,DPLIST_MEMORY_ERROR);
for(i=0;i<size;i++)
{
    dpl_remove_at_index(*list,0,free_element);
}
free(*list);
*list=NULL;
}
								//insert_at_index function
dplist_t * dpl_insert_at_index(dplist_t * list, void * element, int index, bool insert_copy)
{
    // add your code here
  dplist_node_t * ref_at_index, * list_node;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  list_node = malloc(sizeof(dplist_node_t));                          //create a node
  DPLIST_ERR_HANDLER(list_node==NULL,DPLIST_MEMORY_ERROR);
  if(insert_copy == true)
  {
	list_node->element = list->element_copy(element);         //if the insert_copy is true, make a copy of element and add it to the list;
  }
  else
  {
 	 list_node->element = element;
  }
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
							//remove_at_index function (need callback function)
dplist_t * dpl_remove_at_index( dplist_t * list, int index, bool free_element)
{
  dplist_node_t * dummy;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  if (list->head == NULL)  
  { // covers case 1 
    return list;
  }

if(free_element == true)
{
  if(  dpl_size(list)==1 ) 
  { // covers case 2
        list->element_free( &(list->head->element)) ;
        free(list->head);
        list->head=NULL;
        return list;
  }   

  else if( index <= 0 )
  {
     dummy= list->head->next;
     list->head->next->prev=NULL;
     list->element_free( &(list->head->element)) ;
     free(list->head);
     list->head=dummy;
  }  else 
  {
    dplist_node_t * p;
    dplist_node_t * ref_at_index = dpl_get_reference_at_index(list, index);  
    assert( ref_at_index != NULL);
    // pointer drawing breakpoint
    if(index<dpl_size(list)-1)
    {
    p = ref_at_index->prev;
    dummy= ref_at_index->next;
    list->element_free ( &(ref_at_index->element) );
    free(ref_at_index);
    p->next=dummy;
    dummy->prev = p;   //这儿加了一个
    }
    else
    {
    p = ref_at_index->prev;
    list->element_free ( &(ref_at_index->element) );
    free(ref_at_index);
    p->next=NULL;
    }
  }

}

else
{
  if(  dpl_size(list)==1 ) 
  { // covers case 2
        
        free(list->head);
        list->head=NULL;
        return list;
  }   

  else if( index <= 0 )
  {
     dummy= list->head->next;
     list->head->next->prev=NULL;
     free(list->head);
     list->head=dummy;
  }  else 
  {
    dplist_node_t * p;
    dplist_node_t * ref_at_index = dpl_get_reference_at_index(list, index);  
    assert( ref_at_index != NULL);
    // pointer drawing breakpoint
    if(index<dpl_size(list)-1)
    {
    p = ref_at_index->prev;
    dummy= ref_at_index->next;
    free(ref_at_index);
    p->next=dummy;
    dummy->prev = p;   //这儿加了一个
    }
    else
    {
    p = ref_at_index->prev;
    free(ref_at_index);
    p->next=NULL;
    }
  }
}

return list;
}

						//dpl_size function
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
						//get_reference_at_index function
dplist_node_t * dpl_get_reference_at_index( dplist_t * list, int index )
{
    // add your code here
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
						//get_element_at_index function
void * dpl_get_element_at_index( dplist_t * list, int index )
{
    // add your code here
  int count;
  dplist_node_t * dummy;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  if (list->head == NULL ) return 0;
  for ( dummy = list->head, count = 0; dummy->next != NULL ; dummy = dummy->next, count++) 
  { 
    if (count >= index) return (dummy->element);  //这里改了
  }  
  return (dummy->element); //返回的是void类型的指针，在调用这个函数以后可以typecast这个函数的返回值，这样子就能在element里面存储任何类型的数据。
}

int dpl_get_index_of_element( dplist_t * list, void * element )
{
    // add your code here
  int count;
  
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  if (list->head == NULL ) return -1;
  for (count=0;count<dpl_size(list);count++) 
  { 
    if (list->element_compare(dpl_get_element_at_index(list,count),element)==0) return count; // 这里变了
  }  

  return -1;
}

// HERE STARTS THE EXTRA SET OF OPERATORS //

// ---- list navigation operators ----//
  
dplist_node_t * dpl_get_first_reference( dplist_t * list )
{
    // add your code here
    DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
	if(list->head == NULL)
	{
	return NULL;
	}
	else
	{
	return list->head;
	}
}

dplist_node_t * dpl_get_last_reference( dplist_t * list )
{
    // add your code here
    DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
	if(list->head == NULL)
	{
	return NULL;
	}
	else
	{
	return dpl_get_reference_at_index(list, (dpl_size(list)-1));
	}
}

dplist_node_t * dpl_get_next_reference( dplist_t * list, dplist_node_t * reference )
{
    // add your code here
int count=0;
bool inthelist=false;
DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
while(count<dpl_size(list))
{
if(dpl_get_reference_at_index(list,count) == reference)
{
inthelist=true;
}
count++;
}
if(list->head == NULL||reference == NULL||inthelist == false)
{
return NULL;
}
else
{
return reference->next;
}
}

dplist_node_t * dpl_get_previous_reference( dplist_t * list, dplist_node_t * reference )
{
    // add your code here
int count=0;
bool inthelist=false;
DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
while(count<dpl_size(list))
{
if(dpl_get_reference_at_index(list,count) == reference)
{
inthelist=true;
}
count++;
}
if(reference==NULL)
{
    return dpl_get_reference_at_index(list,dpl_size(list)-1);
}
if(list->head == NULL||inthelist == false)
{
return NULL;
}
else
{
return reference->prev;
}
}

// ---- search & find operators ----//  
  
void * dpl_get_element_at_reference( dplist_t * list, dplist_node_t * reference )
{
    // add your code here
int count=0;
bool inthelist=false;
DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
while(count<dpl_size(list))
{
if(dpl_get_reference_at_index(list,count) == reference)     //判断两个pointer是不是相等的方法是不是直接用==？
  {
  inthelist=true;
  }
count++;
}
if(reference == NULL)
  {
  return dpl_get_element_at_index(list, dpl_size(list)-1);;
  }
if(list->head==NULL||inthelist==false)
  {
  return NULL;
  }
else
  {
  return reference->element; 
  }
}

dplist_node_t * dpl_get_reference_of_element( dplist_t * list, void * element )
{
    // add your code here

int index=0;
index=dpl_get_index_of_element(list,element );
DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
if(list->head==NULL||element==NULL||index == -1)
  {
  return NULL;
  }
else  { return  dpl_get_reference_at_index(list, index );}
}

int dpl_get_index_of_reference( dplist_t * list, dplist_node_t * reference )
{
    // add your code here
int count=0;
bool inthelist=false;
while(count<dpl_size(list))
{
if(dpl_get_reference_at_index(list,count) == reference)     //判断两个pointer是不是相等的方法是不是直接用==？
  {
  inthelist=true;
  return count;
  }
count++;
}
if(reference == NULL)  return (dpl_size(list)-1);
if(list->head == NULL||inthelist ==false) return -1;


return -1;
}
  
// ---- extra insert & remove operators ----//

dplist_t * dpl_insert_at_reference( dplist_t * list, void * element, dplist_node_t * reference, bool insert_copy )
{
    // add your code here
int index=0;
index = dpl_get_index_of_reference( list, reference );
DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
if(element == NULL)
  {return list;}
else if(reference==NULL||list->head == NULL)
  {return dpl_insert_at_index(list, element, dpl_size(list),insert_copy);}
else if(index !=-1)
  {return dpl_insert_at_index(list, element, index,insert_copy);}
else return list;
}

dplist_t * dpl_insert_sorted( dplist_t * list, void * element, bool insert_copy )
{
    // add your code here
int count;
dplist_node_t * dummy;
DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
if(list->head == NULL)
  {return dpl_insert_at_index(list, element, dpl_size(list),insert_copy);}
for(count=0,dummy = list->head; dummy!=NULL; dummy = dummy->next)
  {
	if((list->element_compare)(dummy->element,element)!=1) //compare the element and find the position
		{count++;}
    else break;
  }
  return dpl_insert_at_index(list, element, count,insert_copy);
}

dplist_t * dpl_remove_at_reference( dplist_t * list, dplist_node_t * reference, bool free_element )
{
    // add your code here
    int index;
    index = dpl_get_index_of_reference( list, reference );
    DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
    if(index == -1) return list;
    else return dpl_remove_at_index(list,index,free_element);
    
}

dplist_t * dpl_remove_element( dplist_t * list, void * element, bool free_element )
{
    // add your code here
    int index;
    DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
    if(element == NULL) return list;
    index = dpl_get_index_of_element(list,element );
    if(index == -1) return list;
    else return dpl_remove_at_index(list,index,free_element);
}
  
// ---- you can add your extra operators here ----//



