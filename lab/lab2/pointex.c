#include <stdio.h>
#include <stdlib.h>

typedef struct {
short day, month;
unsigned year;
} date_t;

void f( void ) {
int x, y, z;
printf("%d %d %d\n", x, y, z );
}

date_t * date_struct( int day, int month, int year,date_t *da ) {
da->day = (short)day;
da->month = (short)month;
da->year = (unsigned)year;
return da;
}

int main( void ) {
int day, month, year;
date_t *d;
printf("\nGive day, month, year:");
scanf("%d %d %d", &day, &month, &year);
d = date_struct( day, month, year,d );
//f();
printf("\ndate struct values: %d-%d-%d", d->day, d->month, d->year);
return 0;
}

