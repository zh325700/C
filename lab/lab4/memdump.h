#ifndef MEMDUMP_H   //to avoid redefine
#define MEMDUMP_H

#define BYTES_A_ROW         ((int)10)
#define ADDR_COLUMN_WIDTH   ((int)(2*sizeof(void *)))  //char size of an address = num bytes of a pointer * 2 char; (1 byte = hex 2 chars)
#define BYTE_COLUMN_WIDTH   ((int)(BYTES_A_ROW * 3)) // 1 bytes is 2 hex chars + some spaces
#define COLUMN_SEPARATOR    " "

typedef unsigned char byte_t;

void memdump( byte_t *ptr, long size );

#endif /* MEMDUMP_H */

