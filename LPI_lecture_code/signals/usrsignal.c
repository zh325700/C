/*******************************************************************************
*  FILENAME.C  --							       
*									       
*									    
*******************************************************************************/

/*----------------------------------------------------------------------------- 
		include files 
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
		function declarations			      
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
		global variable declarations			       
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
		implementation code			       
------------------------------------------------------------------------------*/
void CatchSignal( int sig )
{
	//signal (SIGUSR1, CatchSignal); // Re-install signal (check with your Linux!)
	//signal (SIGUSR2, CatchSignal);  // Re-install signal (check with your Linux!)
	if ( sig == SIGUSR1 )
		printf("user signal 1 received!\n");
	else if ( sig == SIGUSR2 ) 
		printf("user signal 2 received!\n");
	else
		printf("strange signal received!\n");
	return;
}

static void DoStuff( void )
{
}

static int NotFinished( void )
{
	return 1;
}

int main( void ) 
{
	// run program
	// use kill cmd in terminal to send user signals
	 
	signal (SIGUSR1, CatchSignal); 
	signal (SIGUSR2, CatchSignal);
	
	printf("My process ID = %d\n", getpid() ); 
	while ( NotFinished() )
		DoStuff();
		
	return EXIT_SUCCESS;
}

