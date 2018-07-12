#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define FREQUENCY 5
#define MIN -10
#define MAX 35

int main()
{
time_t oritime;

while(1)
{
struct tm * timedetail;

srand((unsigned)time(NULL));
time(&oritime);
timedetail = localtime(&oritime);
char str[50]="";
strftime(str,50,"%x - %I:%M:%S%p",timedetail);
printf(" Temperature =");
printf("%1.2f @ %s\n",MIN+rand()/(double)(RAND_MAX)*(MAX-MIN),str);
fflush( stdout );
sleep(FREQUENCY);
}

return 0 ;

}
