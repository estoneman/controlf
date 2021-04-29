#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/algos.h"

int main()
{

    char *src = 0;
    long infilesize = 0;
    FILE *infile = fopen("../scrapedhtml/scraped.txt", "r");
    if (infile) {
        fseek(infile, 0, SEEK_END);
        infilesize = ftell(infile);
        fseek(infile, 0, SEEK_SET);
        src = malloc(infilesize);
        if(src)
            fread(src, 1, infilesize, infile);
        fclose(infile);
    }
    char *pat = "as";
    int patternLength = strlen(pat);
    // Shift table and call the horspool function
    int* table = malloc(sizeof(int) * 128);

    int pos = 0, count = 0, numfound = 0;
    shifttable(pat, table, 128);
    while (count < infilesize) {
        pos = horspool(src + count, pat, table);
        printf("%i\n", pos);
        if (pos >= 0) {
            count = count + pos + patternLength;
            numfound++;
        }
        else
            break;
    }
    printf("%i\n", numfound);
    printf("%i\n", s_iter_search(src, pat));
    printf("%i\n", kmp(src, pat));

    free(table);
	return 0;
}
