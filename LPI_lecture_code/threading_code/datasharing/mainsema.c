#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>

#define MAX 10

#define A_VALUE  'a'
#define B_VALUE  'b'

#define THREAD_SHARING 0  // semaphore is shared between threads of the same process and not between processes - no shared memory needs to be created to store the semaphore
#define SEMA_INIT_VALUE 1

// shared array
volatile char data[MAX];

// semaphore
sem_t data_sema;

void err_handler( int err_code, char *msg, char *file_name, char line_nr )
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
	int i, presult;

	printf("\n");
	for ( i = 0 ; i < MAX ; i++ )
	{
		printf("[%c%d]=%c ",c, i, data[i]);
	}
	printf("\n");
}

void *fighter_A( void *id) {
	// this code runs in a thread
	int i, presult;
	while (1)
	{
		presult = sem_wait( &data_sema );
		err_handler( presult, "sem_wait", __FILE__, __LINE__ );
		// set all values in data to A_VALUE but data is unprotected
		for ( i = 0 ; i < MAX ; i++ )
		{
			data[i] = A_VALUE;
		}
		
		usleep(50000);  // 'force' a context switch
		
		print_data('A');
		
		presult = sem_post( &data_sema );
		err_handler( presult, "sem_post", __FILE__, __LINE__ );
		
		usleep(50000);  // 'force' a context switch
	}

	pthread_exit( NULL );
}


void *fighter_B( void *id) {
	// this code runs in a thread
	int i, presult;
	while (1)
	{
		presult = sem_wait( &data_sema );
		err_handler( presult, "sem_wait", __FILE__, __LINE__ );
		// set all values in data to B_VALUE but data is unprotected
		for ( i = 0 ; i < MAX ; i++ )
		{
			data[i] = B_VALUE;
		}
		
		usleep(50000);  // 'force' a context switch
		
		print_data('B');
		
		presult = sem_post( &data_sema );
		err_handler( presult, "sem_post", __FILE__, __LINE__ );
		
		usleep(50000);  // 'force' a context switch
	}

	pthread_exit( NULL );
}

int main(void) {
	int presult;
	pthread_t thread_A, thread_B;
	
	presult = sem_init( &data_sema, THREAD_SHARING,SEMA_INIT_VALUE );        // initial 要在 main  函数里面来完成
	err_handler( presult, "sem_init", __FILE__, __LINE__ );
	
	presult = pthread_create( &thread_A, NULL, &fighter_A, NULL );
	err_handler( presult, "pthread_create", __FILE__, __LINE__ );
	
	presult = pthread_create( &thread_B, NULL, &fighter_B, NULL );
	err_handler( presult, "pthread_create", __FILE__, __LINE__ );

	// important: don't forget to join, otherwise main thread exists and destroys the mutex
	presult= pthread_join(thread_A, NULL);
	err_handler( presult, "pthread_join", __FILE__, __LINE__ );

	presult= pthread_join(thread_B, NULL);
	err_handler( presult, "pthread_join", __FILE__, __LINE__ );

	presult = sem_destroy( &data_sema );
	err_handler( presult, "sem_destroy", __FILE__, __LINE__ );

	pthread_exit(NULL);
}
