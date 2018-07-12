#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define MAX 3

#define A_VALUE  'a'
#define B_VALUE  'b'

// shared array
volatile char data[MAX];

void pthread_err_handler( int err_code, char *msg, char *file_name, char line_nr )
{
  if ( 0 != err_code )
  {
    fprintf( stderr, "\n%s failed with error code %d in file %s at line %d\n", msg, err_code, file_name, line_nr );
    //errno = err_code;
    //perror("Error message: ");
  }
}

void print_data( char c )
{
  int i;
  printf("\n");
  for ( i = 0 ; i < MAX ; i++ )
  {
    usleep(500);  // 'help' a context switch here
    printf("[%c%d]=%c ",c, i, data[i]);
  }
}

void *fighter_A( void *id) {
  // this code runs in a thread
  int i;
  while (1)
  {
    
    // set all values in data to A_VALUE but data is unprotected
    for ( i = 0 ; i < MAX ; i++ )
    {
      data[i] = A_VALUE;

    }
    print_data('A');
    //printf("\n");  
  }
  
  pthread_exit( NULL );
}


void *fighter_B( void *id) {
  // this code runs in a thread
  int i;
  while (1)
  {
    // set all values in data to B_VALUE but data is unprotected
    for ( i = 0 ; i < MAX ; i++ )
    {
      data[i] = B_VALUE;
    } 
    print_data('B'); 
    //printf("\n");  
  }
  
  pthread_exit( NULL );
}

int main(void) {
  int presult;
  pthread_t thread_A, thread_B;
  
  presult = pthread_create( &thread_A, NULL, &fighter_A, NULL );
  pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );
  
  presult = pthread_create( &thread_B, NULL, &fighter_B, NULL );
  pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );
  
  // important: don't forget to join, otherwise main thread exists and destroys process/mutex/...
  presult= pthread_join(thread_A, NULL);
  pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );
  
  presult= pthread_join(thread_B, NULL);
  pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );
  
  pthread_exit(NULL);
}
