#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/algos.h"

// returns the number of occurences of a string within another string, if Aminussymbol
size_t s_iter_search(char *haystack, char *needle) {
    size_t haysize = strlen(haystack), needlesize = strlen(needle), i, j, numfound = 0;
    // entire needle cannot possbily be in the haystack if it is larger than what is being searched
    if (needlesize > haysize) return 0;
    // if the needle is empty or haystack is empty
    if (needlesize <= 0 || haysize <= 0) return 0;
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

void ascii_to_utf8(char *dst, const char *src)
{
        char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;
                } else if (*src == '+') {
                        *dst++ = ' ';
                        src++;
                } else {
                        *dst++ = *src++;
                }
        }
        *dst++ = '\0';
}

void parseForm(char *query_string, size_t query_string_len, char *url, char *search_string) {

    int eq_i1 = 0, eq_i2 = 0, amp_i = 0;

    // find the indexes of each character '=' and '&' respectively
    while (query_string[eq_i1++] != '=');
    amp_i = eq_i1;
    while (query_string[++amp_i] != '&');
    eq_i2 = amp_i;
    while (query_string[eq_i2++] != '=');

    // only start adding the characters between the correct delimiters ('=' and '&')
    for (; eq_i1 < amp_i; eq_i1++) strncat(url, &query_string[eq_i1], 1);
    for (; eq_i2 < query_string_len; eq_i2++) strncat(search_string, &query_string[eq_i2], 1);

}
