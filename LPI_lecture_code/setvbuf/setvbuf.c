
//#include <fcntl.h>
#include <stdio.h>

char str[1024];

int main(void) {
    char a,b,c;
    setvbuf(stdin, str, _IOLBF, 1024);
	
	scanf("%c %c %c", &a, &b, &c);
	//printf("input char: %c %c %c\n", a, b, c);

	printf("%s", str);
    
    return 1;
}
