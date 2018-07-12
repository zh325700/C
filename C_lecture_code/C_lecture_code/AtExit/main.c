#include <stdio.h>
#include <stdlib.h>

static void my_exit1( void );
static void my_exit2( void );

static void my_exit1( void )
{ 
  printf("1: 't is gedaan!\n"); 
}

static void my_exit2( void )
{ 
  printf("2: 't is gedaan!\n"); 
}


int main( void ) 
{ 
  atexit( &my_exit1 );
  atexit( &my_exit2 );
  
  //exit( EXIT_SUCCESS );
  return 0; //works also for return!
} 
