#ifndef UTIL_H
#define UTIL_H

// convert ascii to utf-8
void utf8_decode(char *dst, const char *src);
// write each form value into their own separate variables
void parseForm(char *query_string, size_t query_string_len, char *url, char *search_string);
// get rid of html tags
void rm_html_tags(char *html);

#endif
