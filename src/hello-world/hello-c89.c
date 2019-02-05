
/*
 cl65 --standard c99 -t atari hello-c89.c -o hello-c89.xex  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


int main() {
   
    printf("Hola Mundo!\n");

    int i = 3141;

    printf("int=%d\n", i );


    printf("\npress any key\n");
    (void) cgetc ();

    return 0;
}

