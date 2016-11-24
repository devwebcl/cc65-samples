/* Ghostbusters account generator 
13-oct-2015

#output = You typed (popup): name=MANILLISMO balance=999900; hilo: [I@b88448, hi=$153, lo=$153; sum_chars: $245; euro_chk=$146; account=$268764947[31622546]
#accountout = 31622546

		Update4: otra mas: MANILLISMO 31622546  :P
		Update5: otra cuenta: ATARIWARE 31862546  --> 31062546 ?
		                                (ambos sirven)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

int hilo[2];


int sum_chars(char iname[]) {
	int sum = 0;
	int i;

	for(i=0; iname[i]!='\0'; i++) {
	sum = sum + iname[i];
	}

		  
	return sum & 0xFF;
}


int calc_single_checksum_round(int euro_chk) {
	int a;

	a = euro_chk;
	a <<= 1;
	a ^= euro_chk;
	a <<= 1;
	a ^= euro_chk;
	a <<= 1;
	a <<= 1;
	a ^= euro_chk;
	a <<= 1;
	euro_chk <<= 1;

	if(a & 0x100) euro_chk |= 1;

	return euro_chk;
}


int calc_checksum(int soldi_hi, int soldi_lo, int rounds) {
	int euro_chk;
	int i;

	euro_chk = (soldi_hi + soldi_lo) & 0xFF;
	if(euro_chk == 0) euro_chk = 1;
	for(i = 0; i < rounds; ++i) {
		euro_chk = calc_single_checksum_round(euro_chk) & 0xFF;
	}
	
	return euro_chk;
}


long calc_account(long soldi_hi, long soldi_lo, long euro_chk) {
	long account = 0;

	account |= soldi_hi;
	account <<= 8;
	account |= euro_chk;
	account <<= 8;
	account |= soldi_lo;

	return account;
}


void calc_hilo(long amount) {
	int hi;
	int lo;

	amount /= 100;

	hi = 16 * ((amount / 1000) % 10) + (amount / 100) % 10;
	lo = 16 * ((amount / 10) % 10) + (amount / 1) % 10;

	hilo[0]=hi;
	hilo[1]=lo;
}


int hexi(int v) {
	/*return Integer.valueOf(String.valueOf(v), 16);*/
	/*TODO: devolver un Hex? */
	return v;
}
	
	

void main() {

	char iname[20];   /*manillismo atariware */
	unsigned long ibalance = 0; /* 999990 12123123 */
	int i = 0;
	char accountnumber[20];
	int sum;
	int euro_chk;
	long account;
	char buf[20];


	printf("Ghostbusters Account Generator!\n");
	printf("  -------------------------------\n\n");
	printf("  Original analysis: Zlin\n");
	printf("  http://goo.gl/8jgCvo\n\n");
	printf("  C++ Code: pararaum\n");
	printf("  https://goo.gl/EHS7aZ\n");

	printf("\n  name: ");
	scanf ("%s", &iname);

	printf("  balance: ");
	scanf ("%s", &buf);

	ibalance = atol(buf);

	for(i=0;iname[i]!='\0';i++) {
		iname[i]=toupper(iname[i]);
	}

	calc_hilo(ibalance);

	sum = sum_chars(iname); 

	euro_chk = calc_checksum(hilo[0], hilo[1], sum);

	account = calc_account(hilo[0], hilo[1], euro_chk);

	for(i = 0; i <= 21; i += 6) {
		ultoa (((account >> (i + 3)) & 0x7), buf, 10);
		strcat(accountnumber, buf); /*.toString(16); hexi*/

		ultoa (((account >> i) & 0x7), buf, 10);
		strcat(accountnumber, buf);/* .toString(16); hexi*/
	}


	printf("\n  account number = %s \n\n", accountnumber );

	printf("\n  press any key\n");
	(void) cgetc ();	

}

