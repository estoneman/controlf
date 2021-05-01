#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/algos.h"

// SINGLE ITERATIVE SEARCH
int s_iter_search(char *haystack, char *needle) {
    size_t haysize = strlen(haystack), needlesize = strlen(needle), i, j, numfound = 0;
    // entire needle cannot possbily be in the haystack if it is larger than what is being searched
    // if the needle is empty or haystack is empty
    if (needlesize <= 0 || haysize <= 0 || needlesize > haysize) return 0;

    for (i = 0; i < haysize; i++) {
        if ((*(haystack + i)) == (*needle)) {
            j = 0;
            // while the current character exists in the string AND the current character (either upper or lowercase) of the needle equals the current character of the haystack
            while (*(needle + j) && (*(needle + j)) == (*(haystack + i + j))) j++;
            if (j == needlesize) numfound++;
            // start next iteration of for loop at the position where the while loop iteration left off so there is no repeated statements
            i = i + j - 1;
        }
    }
    return numfound;
}
// END SINGLE ITERATIVE SEARCH

// START HORSPOOL
void shifttable(char pattern[], int bm_table[], int asciiSize) {
    int i, j, pattern_length;
    pattern_length = strlen(pattern);

    for(i = 0; i < asciiSize; i++) {
        bm_table[i] = pattern_length;
    }
    for(j = 0;j < pattern_length - 1; j++) {
        bm_table[(int) pattern[j]] = pattern_length - 1 - j;
    }
}

int horspool (char haystack[], char needle[], int bm_table[]) {
    int iter_pos, k, needlesize, haysize;
    haysize = strlen(haystack);
    needlesize = strlen(needle);
    iter_pos = needlesize - 1;

    while (iter_pos < haysize) {
        k = 0;
        while((k < needlesize) && (needle[needlesize - 1 - k] == haystack[iter_pos - k]))
            k++;
        if (k == needlesize)
            return (iter_pos - needlesize + 1);
        else
            iter_pos += bm_table[haystack[iter_pos]];
    }
    return -1;
}
// END HORSPOOL

// START KNUTH MORRIS PRATT
void computeLPSarray(char *needle, int needlesize, int *lps) {
	int size_ps = 0;	//length of previous longest prefix suffix
	int i;

	lps[0] = 0;	//lps[0] is always 0
	i = 1;

    //loop calculates lps[i] for i = 1 to M-1
	while(i <  needlesize) {
		if(needle[i] == needle[size_ps]) {
			size_ps++;
			lps[i] = size_ps;
			i++;
		}
        // needle[i] != needle[len]
		else {
			if(size_ps != 0) {
				size_ps = lps[size_ps - 1];
			}
            // if len == 0
			else {
				lps[i] = 0;
				i++;
			}
		}
	}
}

int kmp(char *haystack, char *needle)
{
	int needlesize = strlen(needle), haysize = strlen(haystack);

	int *lps = (int*)malloc(needlesize * sizeof(int));
	int j = 0, numfound = 0;

	computeLPSarray(needle, needlesize, lps);

	int i = 0;
	while(i < haysize) {
		if (needle[j] == haystack[i]) { j++; i++; }
		if (j == needlesize) {
            numfound++;
			j = lps[j-1];
		}
		else if(needle[j] != haystack[i]) {
			if(j != 0)
				j = lps[j-1];
			else
				i++;
		}
	}
    return numfound;
	free(lps);
}
// END KNUTH MORRIS PRATT
