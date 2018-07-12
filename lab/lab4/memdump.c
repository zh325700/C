#include <stdio.h>
#include<ctype.h>
#include "memdump.h"
typedef unsigned char byte_t;
#define DATA_SIZE 10

void memdump( byte_t *ptr, long size ) {
  int i=0;
  int j=0;
  //char hexstring[BYTE_COLUMN_WIDTH]="";
  
  // print table header
  printf("\n%-*s", ADDR_COLUMN_WIDTH, "Address");
  printf("%s", COLUMN_SEPARATOR);
  printf("%-*s", BYTE_COLUMN_WIDTH, "Bytes");
  printf("%s", COLUMN_SEPARATOR);
  printf("%-*s\n", BYTES_A_ROW, "Chars");
  
  for( i= 0; i < ADDR_COLUMN_WIDTH; i++)
    printf("-");
  printf("%s", COLUMN_SEPARATOR);
  for( i= 0; i < BYTE_COLUMN_WIDTH; i++)
    printf("-");
  printf("%s", COLUMN_SEPARATOR);
  for( i= 0; i < BYTES_A_ROW; i++)    // number of chars is char-column is exactly BYTES_A_ROW
    printf("-");
  printf("\n");
  
  /*
   * here comes your code !!!!!!!!!
   * 
   * */

  byte_t data[size];
  for(;i<size;i++)
  {
  data[i]=*(ptr+i);
  }

  for(i=0;i<=size/10;i++){	
    	printf("%-*p",ADDR_COLUMN_WIDTH,data+BYTES_A_ROW*i);
	printf("%s", COLUMN_SEPARATOR);
 if(i<size/10)
{
//*hexstring="";
for(j=0;j<BYTES_A_ROW;j++){
	printf("%02X",*(data+j+BYTES_A_ROW*i));
	printf("%s", COLUMN_SEPARATOR);
	//snprintf(hexstring+strlen(hexstring),BYTE_COLUMN_WIDTH,"%02X%s",*(data+j+BYTES_A_ROW*i),COLUMN_SEPARATOR);
}
//printf(" %-*s",BYTE_COLUMN_WIDTH,hexstring);
for(j=0;j<BYTES_A_ROW;j++){
	if(isprint(data[j+BYTES_A_ROW*i]) )
   	{printf("%c",*(data+j+BYTES_A_ROW*i));
   	}
	else{printf("%c",'.');}
}
printf("\n");
}
else{
for(j=0;j<size%10;j++){
	printf("%02X",*(data+j+BYTES_A_ROW*i));
	printf("%s", COLUMN_SEPARATOR);
}
for(j=0;j<size%10;j++){
	if(isprint(data[j+BYTES_A_ROW*i]) )
   	{printf("%c",*(data+j+BYTES_A_ROW*i));
   	}
	else{printf("%c",'.');}
}
printf("\n");
//*hexstring="";
}
	

}

  
}
