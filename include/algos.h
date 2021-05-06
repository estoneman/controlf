#ifndef ALGOS_H
#define ALGOS_H

// SUBSTRING SEARCHING

// NAIVE SEARCH
int search(char *haystack, char *needle);
// SINGLE ITERATIVE SEARCH
int s_iter_search(char *haystack, char *needle);
// BOYER-MOORE HORSPOOL ALGORITHM
int horspool(char src[], char p[], int table[]);
void shifttable(char p[], int t[], int asciiSize);
// KNUTH MOORE PRATT
int kmp(char *haystack, char *needle);
void computeLPSarray(char *needle, int needlesize, int *lps);

#endif
