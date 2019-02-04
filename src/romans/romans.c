/* 
http://mirrors.ctan.org/systems/knuth/dist/tex/tex.web

from https://www.hanshq.net/roman-numerals.html
*/

#include <stdio.h>
#include <conio.h>

void print_roman_int(int n)
{
        const char str[] = "m2d5c2l5x2v5i";

        int j, k;  /* Mysterious indices into str. */
        int u, v;  /* Mysterious numbers. */

        j = 0;
        v = 1000;

       printf("%i=", n);
	   
        while (1) {
                while (n >= v) {
                        putchar(str[j]);
                        n -= v;
                }

                if (n <= 0) {
                        return;  /* Nonpositive input produces no output. */
                }

                k = j + 2;
                u = v / (str[k - 1] - '0');

                if (str[k - 1] == '2') {
                        k += 2;
                        u = u / (str[k - 1] - '0');
                }

                if (n + u >= v) {
                        putchar(str[k]);
                        n += u;
                } else {
                        j += 2;
                        v = v / (str[j - 1] - '0');
                }
        }
}


void main() {
   
    printf("\nKnuth's solution for Roman Numbers\nwritten in TeX manual \nhttp://mirrors.ctan.org/systems/knuth/dist/tex/tex.web\n\n");

    print_roman_int(2019);	
    printf("\n\n");
    print_roman_int(3141);	
    printf("\n\n");
    print_roman_int(1);	
    printf("\n\n");
	
    printf("\npress any key\n");
    (void) cgetc ();

}


