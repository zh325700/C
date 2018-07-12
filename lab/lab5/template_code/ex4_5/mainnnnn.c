#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

dplist_t * list = NULL;

void dpl_print( dplist_t * list );

// callback functions
void * element_copy(void * element);     // Duplicate 'element'; If needed allocated new memory for the duplicated element.
void   element_free(void ** element);   // If needed, free memory allocated to element
int    element_compare(void * x, void * y);     // Compare two element elements; returns -1 if x<y, 0 if x==y, or 1 if x>y 

int main(void)
{
  
  
  int * a;
  int * b;
  int x,aa;
  void* cc;
  dplist_node_t *  t;
  int index;
  a = malloc( sizeof(int) );
  assert(a != NULL);
  
  list = dpl_create(element_copy,element_free,element_compare);

  *a = 0;
  dpl_insert_at_index(list,a,5,true);

  *a = 1;
  dpl_insert_at_index(list,a,5,true);

  *a = 3;
  dpl_insert_at_index(list,a,5,true);

  b = malloc( sizeof(int) );
  assert(a != NULL);

  *b = 5;
  dpl_insert_at_index(list,b,10,true);  

  *b = 7;
  dpl_insert_at_index(list,b,10,true); 
  
//  dpl_print(list);

   *b = 4;
   dpl_insert_sorted( list, b, true);     //for insert_sorted
   
   *b = 9;
   dpl_insert_sorted( list, b, true);     //for insert_sorted
   
   dpl_print(list);
   t=dpl_get_last_reference(list);        //print reversed list
   for(int count=0;count<dpl_size(list);count++)
   {
       printf("%d ",*(int *)dpl_get_element_at_reference( list, t));
      t=dpl_get_previous_reference(list,t);
   }
   printf("\n");
   t = dpl_get_reference_of_element(list,b);
   *a=10;
   dpl_insert_at_reference(list,a,t,true);  //insert 10
   t = dpl_get_previous_reference(list, t);       
   dpl_remove_at_reference(list,t,true);
   dpl_print(list);
   
   dpl_free(&list,true);
//    x = 3;
//
//   index = dpl_get_index_of_element(list, &x);
//
//   if (index == -1)
//   {
//     printf("element %d is not found\n", x);
//   }
//   else
//   {
//     printf("element %d found at index %d\n", x, index);
//   }

//   x = 3;
//   t= dpl_get_reference_of_element( list,&x);
//   index=dpl_get_index_of_reference(list, t);
//
//   if (index == -1)
//   {
//     printf("element %d is not found\n", x);
//   }
//   else
//   {
//     printf("element %d found at index %d\n", x, index);
//   }

  //remove elements from the list
//   list = dpl_remove_at_index(list, -1,true);
//   list = dpl_remove_at_index(list, -1,true);
//   list = dpl_remove_at_index(list, 10,true);
//   dpl_print(list);
  
//   t = dpl_get_first_reference(list);
//   index=dpl_get_index_of_reference(list, t);
//   printf("index found at index %d\n", index);

//   t = dpl_get_last_reference(list);
//   index=dpl_get_index_of_reference(list, t);
//   printf("index found at index %d\n", index);




//   aa=2;
//   t=dpl_get_next_reference(list, dpl_get_reference_at_index(list,aa));
//   cc =dpl_get_element_at_reference(list,t);
//   aa++;
//   printf("index element %d = %d\n", aa, *(int *)cc);

//   aa=3;
//   t=dpl_get_previous_reference(list, dpl_get_reference_at_index(list,NULL));
//   cc =dpl_get_element_at_reference(list,t);                         //because t is NULL so the last element will be return under this function
//   aa--;
//   printf("\nindex element %d = %d\n", aa, *(int *)cc);


//   t = dpl_get_first_reference(list);
//   cc=dpl_get_element_at_reference( list,t );
//   printf("\nreference element = %d\n", *(int *)cc);
  
//   x = 3;
//   t= dpl_get_reference_of_element( list,&x);
//   cc=dpl_get_element_at_reference( list,t );
//   printf("reference element = %d\n", *(int *)cc);
  
//   *a = 2;
//   x=1;
//   t= dpl_get_reference_of_element( list,&x);
//   dpl_insert_at_reference( list,a,t,true);
//   dpl_print(list);
  
 
//   *a = 2;
//   aa=1;                                                   
//   t=dpl_get_reference_at_index  (list,aa);
//   cc=dpl_get_element_at_reference( list,t);
//   printf("reference element = %d\n", *(int *)cc);
//   dpl_insert_at_reference( list,a,t,true);
//   x=dpl_get_index_of_reference(list,t);
//   printf("index of element = %d\n", x);



//   *a =3;
//   index=dpl_get_index_of_element(list,a );
//   printf("index of element = %d\n", index);
//   t=dpl_get_reference_of_element(list,a);
//   cc=dpl_get_element_at_reference(list,t);        //这里有问题，当reference = NULL 的时候返回的element有问题
//   if(cc==NULL){printf("\nnihao");}
//   printf("reference element = %d\n", *(int *)cc);
//   dpl_print(list);




  
//   aa=3;                                                 
//   t=dpl_get_reference_at_index  (list,aa);
//   assert(t!=NULL);
//   cc=dpl_get_element_at_reference( list,t);
//   printf("reference element = %d\n", *(int *)cc);
//   dpl_remove_at_reference( list,t, true);
//   dpl_print(list);

    


  // //x = 3; 
//   x = 6; 
//   dpl_remove_element(list, &x,true);
//   dpl_print(list);


//  dpl_free( &list,true);                        

  free(a); 
  free(b);
  dpl_print(list);
  // check Valgrind: after freeing the entire list, there is still a memory leak!!!
  
  return 0;
  
}

void dpl_print( dplist_t * list )
{
  void * element;
  int i, length;
  if( list==NULL) printf("List is null\n");
  length = dpl_size(list);
  if( length==0) printf("List is empty\n");
  
  for ( i = 0; i < length; i++)    
  {
    element = dpl_get_element_at_index( list, i );
    printf("index element %d = %d\n", i, *(int *)element);
  }
}

void * element_copy(void * element)
{
        
        int * bb= malloc(sizeof(int));
        *bb = * (int *)element;
        
        return bb;
    
} 


void   element_free(void ** element)// If needed, free memory allocated to element
{
    free(*element);
    *element=NULL;   
}

int    element_compare(void * x, void * y)      // Compare two element elements; returns -1 if x<y, 0 if x==y, or 1 if x>y 
{
    int xx= * (int *)x;
    int yy= * (int *)y;
    


        if(xx< yy ) return -1;

        else if(xx== yy) return 0;
    
    else return 1;

}
