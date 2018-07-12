
/*
 *  FILENAME: main.c
 *
 * Version 
 * An implementation of a stack datastructure;
 * Generic data types: type 'void *';
 * Generic element type: type 'void *'
 *
 */

#include <assert.h>
#include <stdlib.h>
#include "stack.h"

#define MAX_STACK_SIZE	10

struct the_stack {
  element_t data[MAX_STACK_SIZE];
  int top;
  destroy_element_func *destroy_element; // callback function
  copy_element_func *copy_element;
};

typedef struct the_stack the_stack_t;

static void check_assert( the_stack_t * const );

err_code_t stack_create( stack_t *s, destroy_element_func *destroy_element, copy_element_func *copy_element )
{
  the_stack_t *t;

  t = (the_stack_t *) malloc ( sizeof(the_stack_t) );
  if (t == NULL )
  {
    *s = NULL;
    return ERR_MEM;
  }
  
  t->top = 0;	/* indicates empty stack */
 
  t->destroy_element = destroy_element;
  t->copy_element = copy_element;
  
  *s = (stack_t)t;
  return ERR_NONE;
}


err_code_t stack_free( stack_t *s )
{
  the_stack_t *t = (the_stack_t *)*s;
  
  while ( t->top != 0 ) {
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    t->top--; 
    t->destroy_element( t->data[t->top] );// CLEANUP THE MEMORY ASSIGNED TO THE ELEMENT
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  }
  
  free(*s);
  *s = NULL;
  return ERR_NONE;
}

err_code_t stack_push( stack_t s, const element_t element )
{
  the_stack_t *t = (the_stack_t *)s;
  
  check_assert( t );
  
  if (  t->top == MAX_STACK_SIZE )
    return ERR_FULL;
  
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  t->data[t->top++] = t->copy_element( element ); // CALL CALLBACK FUNC TO MAKE A DEEP COPY 
  
  //t->data[t->top++] = element; // NOT POSSIBLE IF = IS NOT DEFINED FOR THIS TYPE OF ELEMENT  
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  return ERR_NONE;
}


err_code_t stack_pop( stack_t s )
{
  the_stack_t *t = (the_stack_t *)s;
  
  check_assert( t );
  
  if ( t->top == 0 )
    return ERR_EMPTY;
  
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  t->top--; 
  t->destroy_element( t->data[t->top] );// CALL CALLBACK FUN TO CLEANUP THE MEMORY ASSIGNED TO THE ELEMENT
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  return ERR_NONE;
}


err_code_t stack_top( const stack_t s, element_t * element )
{
  the_stack_t *t = (the_stack_t *)s;
  
  check_assert( t );
  
  if ( t->top == 0 )
    return ERR_EMPTY;
  
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	///	
  *element = t->copy_element( t->data[t->top-1] ); // RETURN DEEP COPY
  //*element = t->data[t->top-1]; // RETURN JUST A POINTER
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	
  
  return ERR_NONE;
}

unsigned int stack_size( const stack_t s )
{
  the_stack_t *t = (the_stack_t *)s;
  
  check_assert( t );
  
  return ( t->top );
}

static void check_assert( the_stack_t * const s )
{
  assert( (s->top >= 0) && (s->top <= MAX_STACK_SIZE) ); 
}