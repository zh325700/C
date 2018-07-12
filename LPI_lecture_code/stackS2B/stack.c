/*******************************************************************************
 *  FILENAME: main.c
 *
 * Version
 * An implementation of a stack datastructure;
 * - Pointer list implementation of a stack
 *
 *******************************************************************************/

//#define DEBUG

#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "stack.h"

#ifdef DEBUG
#define DEBUG_PRINT(...) 							\
do {					  					\
  printf("In %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	\
  printf(__VA_ARGS__);								\
  } while(0)
#else
  #define DEBUG_PRINT(...) (void)0
#endif
  
  
err_code_t stack_create( stack_t ** stack )
{
  stack_t *t;
    
  DEBUG_PRINT( "\nstack_create() called\n" );
    
  t = (stack_t *) malloc ( sizeof(stack_t) );
  if (t == NULL )
  {
    *stack = NULL;
    return ERR_MEM;
  }
    
  t->top = NULL;	/* indicates empty stack */
  t->num_elements = 0;
    
  *stack = t;
  return ERR_NONE;
}
  
  
err_code_t stack_free( stack_t ** stack )
{
  DEBUG_PRINT( "\nstack_free() called\n" );
    
  node_t *temp;
  stack_t *t = *stack;
    
  // clean up memory
  while ( t->top != NULL )
  {
    temp = t->top->next;
    free( t->top );
    t->top = temp;
  }
    
  free(*stack);
  *stack = NULL;
  return ERR_NONE;
}
  
  
err_code_t stack_push( stack_t * stack , const element_t element )
{
  node_t *temp;
    
  DEBUG_PRINT( "\nstack_push() called\n" );
    
  temp = (node_t *) malloc( sizeof(node_t) );
  if ( temp == NULL )
    return ERR_MEM;
    
  temp->element = element;
  temp->next = stack->top;
  stack->top = temp;
    
  stack->num_elements++;
    
  return ERR_NONE;
}
  
  
err_code_t stack_pop( stack_t * stack )
{
  node_t *temp;
    
  DEBUG_PRINT( "\nstack_pop() called\n" );
    
  if ( stack->top == NULL )
    return ERR_EMPTY;
    
  temp = stack->top;
  stack->top = stack->top->next;
  free( temp );
  
  stack->num_elements--;
    
  return ERR_NONE;
}
  
  
err_code_t stack_top( const stack_t * stack, element_t * element )
{
    
  DEBUG_PRINT( "\nstack_top() called\n" );
   
  if ( stack->top == NULL )
    return ERR_EMPTY;
    
  *element = stack->top->element;
  return ERR_NONE;
}
  
  
unsigned int stack_size( const stack_t * stack )
{
  
  DEBUG_PRINT( "\nstack_size() called\n" );
    
  return ( stack->num_elements);
}

err_code_t stack_save( const stack_t * stack, const char *path  )
{  
  // don't use fprintf & fscanf ==> results in text file
  // check if file size corresponds with stack size: using fseek to compute #elements will not work anymore (file with chars!) 
  // read all elements BUT reading them in reverse order is no longer an easy job (file with chars!)
  
  node_t *t = stack->top;
  FILE *fp;
  int result;
  
  fp = fopen( path, "w" );
  if ( ! fp ) 
  {
    perror("File open failed: ");
    return ERR_FILE;
  }
  
  result = fwrite( &(stack->num_elements), sizeof(stack->num_elements), 1, fp );
  if ( result < 1 ) {
    perror("File write failed: ");
    fclose( fp );
    return ERR_FILE;
  }
  
  while ( t != NULL )
  {		
    // write element to file
    result = fwrite( &(t->element), sizeof(element_t), 1, fp );
    if ( result == -1 ) {
      perror("File write failed: ");
      fclose( fp );
      return ERR_FILE;
    }
    t = t->next;
  }
  
  if ( fclose(fp) == EOF ) {
    perror("File close failed: ");
    return ERR_FILE;
  }	
  
  return ERR_NONE;
}


err_code_t stack_restore( stack_t * stack, const char *path  )
{
  element_t element;
  FILE *fp;
  int result;
  int count_elements;
  int offset;
  err_code_t err;
  
  // open file
  fp = fopen( path, "r" );
  if ( ! fp ) {
    perror("File open failed: ");
    return ERR_FILE;
  }
  
  // read size of stack from file
  result = fread( &count_elements, sizeof(stack->num_elements), 1, fp );
  if ( result < 1 ) {
    perror("File read failed: ");
    fclose( fp );
    return ERR_FILE;
  }
  
  result  = fseek( fp, 0, SEEK_END );
  if ( result == -1 ) {
    perror("File fseek failed: ");
    fclose( fp );
    return ERR_FILE;
  }
  
  // double check size of stack: fseek doesn't return #bytes ==> ftell() must be used
  if ( count_elements != ( ftell(fp) - sizeof(stack->num_elements)) / sizeof(element_t) ) {
    fprintf(stderr, "File contains not enough elements\n");
    fclose( fp );
    return ERR_FILE;
  }
  
  // read all elements BUT read them in reverse order for pushing on the stack
  for ( offset = sizeof(element_t); count_elements; offset += sizeof(element_t), count_elements-- )
  {
    result  = fseek( fp, -offset, SEEK_END );
    if ( result == -1 ) {
      perror("File fseek failed: ");
      fclose( fp );
      return ERR_FILE;
    }	
    
    result = fread( &element, sizeof(element_t), 1, fp );
    if ( result < 1 ) {
      perror("File read failed: ");
      fclose( fp );
      return ERR_FILE;
    }
    
   
    if ( (err = stack_push( stack, element ) ) != ERR_NONE ) {
      fclose( fp );
      return err;
    }
    
  }
  
  if ( fclose(fp) == EOF ) {
    perror("File close failed: ");
    return ERR_FILE;
  }	
  
  return ERR_NONE;
}





