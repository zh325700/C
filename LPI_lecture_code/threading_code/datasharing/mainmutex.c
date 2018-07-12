#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define MAX 3

#define A_VALUE  'a'
#define B_VALUE  'b'

// shared array
volatile char data[MAX];
pthread_barrier_t sync_here;


// mutex
pthread_mutex_t data_mutex= PTHREAD_MUTEX_INITIALIZER;//PTHREAD_ERRORCHECK_MUTEX_INITIALIZER;

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
	int i, presult;

	printf("\n");
	for ( i = 0 ; i < MAX ; i++ )
	{
		printf("[%c%d]=%c ",c, i, data[i]);
	}
	//printf("\n");
}

void *fighter_A( void *id) {
	// this code runs in a thread
	int i, presult;
	while (1)
	{
        //presult = pthread_mutex_init(&data_mutex,NULL);
		presult = pthread_mutex_lock( &data_mutex );
		pthread_err_handler( presult, "pthread_mutex_lock", __FILE__, __LINE__ );
		// set all values in data to A_VALUE but data is unprotected
		for ( i = 0 ; i < MAX ; i++ )
		{
			data[i] = A_VALUE;
		}
		
		usleep(50000);  // 'force' a context switch  当没有mutx 保护的时候， 这个语句会转换线程，就是说data[]里面的东西不再是A线程想要的了，可能，座椅在对globle variable 进行操作的时候要用到critical section
		
		print_data('A');
		
		presult = pthread_mutex_unlock( &data_mutex );
		pthread_err_handler( presult, "pthread_mutex_unlock", __FILE__, __LINE__ );
		
		usleep(1000000);  // 'force' a context switch
        pthread_barrier_wait( &sync_here );
	}

	pthread_exit( NULL );
}


void *fighter_B( void *id) {
	// this code runs in a thread
	int i, presult;
	while (1)
	{
        //presult = pthread_mutex_init(&data_mutex,NULL);
		presult = pthread_mutex_lock( &data_mutex );
		pthread_err_handler( presult, "pthread_mutex_lock", __FILE__, __LINE__ );
		// set all values in data to B_VALUE but data is unprotected
		for ( i = 0 ; i < MAX ; i++ )
		{
			data[i] = B_VALUE;
		}
		
		usleep(50000);  // 'force' a context switch         
		
		print_data('B');
		
		presult = pthread_mutex_unlock( &data_mutex );
		pthread_err_handler( presult, "pthread_mutex_unlock", __FILE__, __LINE__ );
		
		usleep(50000);  // 'force' a context switch
        pthread_barrier_wait( &sync_here );
	}

	pthread_exit( NULL );
}

int main(void) {
	int presult;
	pthread_t thread_A, thread_B;
	
//    presult = pthread_mutex_init(&data_mutex,NULL);
//    pthread_err_handler( presult, "pthread_initialization", __FILE__, __LINE__ );
	presult = pthread_barrier_init( &sync_here, NULL, 2 );
	pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );	
    
	presult = pthread_create( &thread_A, NULL, &fighter_A, NULL );
	pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );
	
	presult = pthread_create( &thread_B, NULL, &fighter_B, NULL );
	pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );

	// important: don't forget to join, otherwise main thread exists and destroys the mutex
	presult= pthread_join(thread_A, NULL);
	pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );

	presult= pthread_join(thread_B, NULL);
	pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );

	presult = pthread_mutex_destroy( &data_mutex );
	pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );
    
    presult = pthread_barrier_destroy( &sync_here );                     
	pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );

	pthread_exit(NULL);
}
