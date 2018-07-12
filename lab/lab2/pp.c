#include<stdio.h>
#include<stdlib.h>
void swap_pointers(int **p1, int **p2);

int main(void)
{
int a = 1;
int b = 2;
int *p=&a;
int *q=&b;


printf("the address of p = %p and q=%p\n",p,q);

swap_pointers(&p,&q);
printf("address of p = %p and q= %p\n",p,q);

return 0 ;
}

void swap_pointers(int **p1 , int **p2)
{
 int *swap;
 swap = *p1;
 *p1= *p2;
 *p2 = swap;
}
