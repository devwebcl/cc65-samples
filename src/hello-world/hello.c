/* This is a trivial example program
   for use with CC65. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


void main() {
   
	unsigned char string1[] =
	{
		97
    };

	char *string2 = "hola";

    printf("Hello world!\n\n" );
    printf("free mem=%d \n\n", _heapmemavail() );
    printf("free mem=%d \n\n", _heapmaxavail() );
    printf("float=%d\n", sizeof(float) );
    printf("long=%d\n", sizeof(long) );
    printf("double=%d\n", sizeof(double) );
    printf("int=%d\n", sizeof(int) );
    printf("short=%d\n", sizeof(short) );
    printf("char=%d\n", sizeof(char) );
    printf("string1=%s\n", &string1 );
    printf("string2=%s\n", string2 );

    printf("\npress any key\n");
    (void) cgetc ();


}

