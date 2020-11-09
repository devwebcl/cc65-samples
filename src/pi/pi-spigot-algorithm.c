#include <stdio.h>
 
// https://www.z88dk.org/wiki/doku.php?id=libnew:examples:pi#original_source_code

//#pragma output CRT_ORG_CODE = 0
//#pragma output REGISTER_SP  = 0
 
int main()
{
   long r[2800 + 1];
   long i, k;
   long b, d;
   long c;
 
   //#asm
//di
//ticks_start:
   //#endasm
 
   c = 0;
 
   for (i = 0; i < 2800; i++)
      r[i] = 2000;
 
   for (k = 2800; k > 0; k -= 14)
   {
      d = 0;
      i = k;
 
      while (1) 
      {
         d += r[i] * 10000;
         b = i * 2 - 1;
 
         r[i] = d % b;
         d /= b;
 
         i--;
         if (i == 0) break;
 
         d *= i;
      }
 
      printf("%.4d", (int)(c + d / 10000));
 
      c = d % 10000;
   }
 
   //#asm
//ticks_end:
   //#endasm
 
   return 0;
}

