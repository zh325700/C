#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define FREQUENCY 5
#define MIN -10
#define MAX 35

void random_number();

int main()
{
time_t oritime;
struct tm * timedetail;

time(&oritime);
timedetail = localtime(&oritime);

srand((unsigned)time(NULL));

while(1)
{
printf(" Temperature =");
printf("%1.2f @ %s\n",-10+rand()/(double)(RAND_MAX)*45,asctime(timedetail));
fflush( stdout );
sleep(FREQUENCY);
}

return 0 ;

}
