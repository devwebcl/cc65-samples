#include <stdio.h>
#include <conio.h>

int add (int,int); /* function prototype for add */

void main()
{
    printf("%d\n",add(3, 2));
	
    printf("\npress any key\n");
    (void) cgetc ();
	
}

int add(int i, int j)
{
    return i+j;
}

