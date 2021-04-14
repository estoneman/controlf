#ifndef ALGOS_H_
#define ALGOS_H_

// single iterative search for a substring(needle) inside a string(haystack)
size_t s_iter_search(char *haystack, char *needle);
// convert ascii to utf-8
void ascii_to_utf8(char *dst, const char *src);
// write each form value into their own separate variables
void parseForm(char *query_string, size_t query_string_len, char *url, char *search_string);

#endif
