#include <stdio.h>
// #include <stdlib.h>
#include <conio.h>

//int main(int argc, char *argv[])
int main()
{
    // Taken from here: http://denniskubes.com/2012/08/19/pointers-and-arrays-in-c/
    // https://gist.githubusercontent.com/mrnugget/6208844/raw/e941ed034bca6f3947f9868dd14b545c620867b8/pointers_and_arrays.c
	
	//pointer basic
	int num = 0; 
	int *pi = &num; //points address of num
		
    // initialize an array of ints
    int numbers[5] = {1, 2, 3, 4,5};

    // standard array notation
    int *ptr1 = numbers;
    int val1 = numbers[0];

    // address of array notation
    int *ptr2 = &numbers[0];
    int val2 = *(&numbers[0]);

    // pointer + increment notation
    int *ptr3 = numbers + 0;
    int val3 = *(numbers + 0);

     /*
    pointers.c(35): Error: Expression expected
    pointers.c(35): Error: Undefined symbol: `cptr'
 	*/
    //variables deben ser declaradas al principio
    char *cptr = "Hello there!";
    char carr[] = "Hello there!";
    char *np = cptr;

    struct person {
        int age;
        char *name;
    };
    struct person per;

    struct person *pptr = &per;

	printf("Address of num: %d Value: %d\n\n",&num, num); printf("Address of pi: %d Value: %d\n",&pi, pi);	

    printf("\n\nprint out the address stored in the pointers\n");
    printf("(void *)ptr1 = %p\n", (void *)ptr1);
    printf("(void *)ptr2 = %p\n", (void *)ptr2);
    printf("(void *)ptr3 = %p\n", (void *)ptr3);

    printf("\n\nprint out the value at the pointer addresses\n");
    printf("val1 = %d\n", val1);
    printf("val2 = %d\n", val1);
    printf("val3 = %d\n", val1);


    printf("\n\nchar arrays and char pointers:\n");

    printf("cptr = %s\n", cptr);
    printf("carr = %s\n", carr);

    printf("cptr[1] = %c (with %%c)\n", cptr[1]);
    printf("carr[1] = %c (with %%c)\n", carr[1]);

    printf("*cptr = %c (with %%c)\n", *cptr);
    printf("*(cptr + 1) = %c (with %%c)\n", *(cptr + 1));

    printf("*cptr = %d (with %%d)\n", *cptr);
    printf("*(cptr + 1) = %d (with %%d)\n", *(cptr + 1));

    printf("\n\nComparing the integer values of characters:\n");
    printf("*cptr == carr[0]: %d\n", *cptr == carr[0]);             // True (1)
    printf("*cptr == carr[1]: %d\n", *cptr == carr[1]);             // False (0)
    printf("*(cptr + 1) == carr[1]: %d\n", *(cptr + 1) == carr[1]); // True (1)

    // Setting a new pointer to the memory address of `cptr`
    printf("\n\nLooping through a pointer until hitting 0 character:\n");

    while(*np != 0) {
        printf("*np: %c\n", *np++);
    }
    printf("After loop - np: %s\n", np);
    printf("After loop - cptr: %s\n", cptr);


    // Taken from here: http://denniskubes.com/2012/08/16/the-5-minute-guide-to-c-pointers/
    printf("\n\nStructs and pointers:\n");

    per.age = 31;
    per.name = "Gregorio Ramone";

    printf("per.age=%d, per.name=%s\n", per.age, per.name);


    printf("(*pptr).age=%d, (*pptr).name=%s\n", (*pptr).age, (*pptr).name);
    printf("pptr->age=%d, pptr->name=%s\n", pptr->age, pptr->name);
	
    printf("\npress any key\n");
    (void) cgetc ();
   
    return 0;	
}
