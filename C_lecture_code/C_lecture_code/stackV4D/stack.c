
#include <stdlib.h>
#include "stack.h"

#ifdef DEBUG
  #include <stdio.h>
#endif


#ifdef DEBUG
#define DEBUG_PRINT(x,y,...) 								\
  do {					  						\
    printf("In %s in function %s() at line %d: ", __FILE__, __func__, __LINE__);		\
    printf(__VA_ARGS__);								\
  } while(0)
#else
  #define DEBUG_PRINT(...) (void)0
#endif


err_code_t stack_create( stack_t **s )
{
  stack_t *t;
  
  DEBUG_PRINT( aaa, bbbb, "\nstack_create() called\n" );
  t = (stack_t *) malloc ( sizeof(stack_t) );
  if (t == NULL )
  {
    *s = NULL;
    return ERR_MEM;
  }
  
  t->top = 0;	/* indicates empty stack */
  
  *s = t;
  return ERR_NONE;
}


err_code_t stack_free( stack_t **s )
{
  DEBUG_PRINT( "\nstack_destroy() called\n" );
  free(*s);
  *s = NULL;
  return ERR_NONE;
}


err_code_t stack_push( stack_t * s, const element_t element )
{
  DEBUG_PRINT( "\nstack_push() called\n" );
  if (  s->top == MAX_STACK_SIZE )
    return ERR_FULL;
  
  s->data[s->top++] = element;
  return ERR_NONE;
}


err_code_t stack_pop( stack_t * s )
{
  DEBUG_PRINT( "\nstack_pop() called\n" );
  if ( s->top == 0 )
    return ERR_EMPTY;
  
  s->top--;
  return ERR_NONE;
}

err_code_t stack_top( const stack_t * s, element_t * element )
{
  DEBUG_PRINT( "\nstack_top() called\n" );
  if ( s->top == 0 )
    return ERR_EMPTY;
  *element = s->data[s->top-1];
  return ERR_NONE;
}


unsigned int stack_size( const stack_t * s )
{
  DEBUG_PRINT( "\nstack_size() called\n" );
  return ( s->top );
}





