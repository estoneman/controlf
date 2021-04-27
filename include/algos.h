#ifndef ALGOS_H
#define ALGOS_H

// single iterative search for a substring(needle) inside a string(haystack)
size_t s_iter_search(char *haystack, char *needle);
// convert ascii to utf-8
void utf8_decode(char *dst, const char *src);
// write each form value into their own separate variables
void parseForm(char *query_string, size_t query_string_len, char *url, char *search_string);
// get rid of html tags
void plain_text(char *html);

#endif
