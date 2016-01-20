/* This is a trivial example program
   for use with CC65. */


#include <stdlib.h>
#include <string.h>
#include <conio.h>


main() {  
   
    printf("Hello world!\n\n" );
	printf("free mem=%d \n\n", _heapmemavail() );
    printf("float=%d\n", sizeof(float) );
    printf("long=%d\n", sizeof(long) );
    printf("double=%d\n", sizeof(double) );
    printf("int=%d\n", sizeof(int) );
    printf("short=%d\n", sizeof(short) );
    printf("char=%d\n", sizeof(char) );

    printf("\npress any key\n");
    (void) cgetc ();


}
