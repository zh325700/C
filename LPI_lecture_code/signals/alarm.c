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
	if ( sig == SIGALRM )
	{
		printf("Wake up! Wake up! Wake up! \n");
		//set alarm
		alarm(5);
	}
	return;
}

static void DoStuff( void )
{
  printf("Sleeping ...\n");
  sleep(1);
}

static int NotFinished( void )
{
	return 1;
}

int main( void ) 
{
	// run program
	// use kill cmd in terminal to send user signals
	 
	signal (SIGALRM, CatchSignal); 

	//set alarm
	alarm(5);
	
	printf("My process ID = %d\n", getpid() ); 
	while ( NotFinished() )
		DoStuff();
		
	return EXIT_SUCCESS;
}

