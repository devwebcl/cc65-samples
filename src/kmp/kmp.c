// https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
// from https://www.geeksforgeeks.org/kmp-algorithm-for-pattern-searching/

// C++ program for implementation of KMP pattern searching 
// algorithm 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


//#include <bits/stdc++.h> 

// void computeLPSArray(char* pat, int M, int* lps); 

// Fills lps[] for given patttern pat[0..M-1] 
void computeLPSArray(char* pat, int M, int* lps) 
{ 
	// length of the previous longest prefix suffix 
	int len = 0; 

	int i = 1; 

	lps[0] = 0; // lps[0] is always 0 

	// the loop calculates lps[i] for i = 1 to M-1 
	while (i < M) { 
		if (pat[i] == pat[len]) { 
			len++; 
			lps[i] = len; 
			i++; 
		} 
		else // (pat[i] != pat[len]) 
		{ 
			// This is tricky. Consider the example. 
			// AAACAAAA and i = 7. The idea is similar 
			// to search step. 
			if (len != 0) { 
				len = lps[len - 1]; 

				// Also, note that we do not increment 
				// i here 
			} 
			else // if (len == 0) 
			{ 
				lps[i] = 0; 
				i++; 
			} 
		} 
	} 
} 


// Prints occurrences of txt[] in pat[] 
void KMPSearch(char* pat, char* txt, int M, int* lps) {

    const int A = M;
	// create lps[] that will hold the longest prefix suffix 
	// values for pattern 
	//int lps[A]; 
	

//	int M = strlen(pat); 
	int N = strlen(txt); 
	 


	int i = 0; // index for txt[] 
	int j = 0; // index for pat[] 

	// Preprocess the pattern (calculate lps[] array) 
	computeLPSArray(pat, M, lps); 

	while (i < N) { 
		if (pat[j] == txt[i]) { 
			j++; 
			i++; 
		} 

		if (j == M) { 
			printf("Found pattern at index %d ", i - j); 
			j = lps[j - 1]; 
		} 

		// mismatch after j matches 
		else if (i < N && pat[j] != txt[i]) { 
			// Do not match lps[0..lps[j-1]] characters, 
			// they will match anyway 
			if (j != 0) 
				j = lps[j - 1]; 
			else
				i = i + 1; 
		} 
	} 
} 


// Driver program to test above function 
int main() 
{ 
	char txt[] = "ABABDABACDABABCABAB";
	char pat[] = "ABABCABAB";
	
	//TODO: kmp.c(102): Error: Constant integer expression expected
    int lps[9];  //strlen(pat)
	
	KMPSearch(pat, txt, strlen(pat), lps); // M = strlen(pat)
	
    printf("\npress any key\n");
    (void) cgetc ();
	
	return 0; 
} 
