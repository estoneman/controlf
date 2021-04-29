#include <ctype.h>
#include <string.h>

static const char _OPENED = '<', _CLOSED = '>';

// UTILITY FUNCTIONS
void utf8_decode(char *dst, const char *src)
{
        char a, b;
        while (*src) {
            if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) {
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
            }
            else if (*src == '+') {
                *dst++ = ' ';
                src++;
            }
            else {

                *dst++ = tolower(*src++);
            }
        }
        *dst++ = '\0';
}

void parseForm(char *query_string, size_t query_string_len, char *url, char *search_string) {

    size_t eq_i1 = 0, eq_i2 = 0, amp_i = 0;

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

void rm_html_tags(char *html) {
    char* result = html;

    size_t idx = 0, nRead = strlen(html), opened = 0, i;
    for(i = 0; i < nRead; i++) {
        // _isspace = (isspace(*(result + i))) ? 1 : 0;
        if (*(result + i) == _OPENED) {
            opened = 1; // true
        }
        else if (*(result + i) == _CLOSED) {
            opened = 0; // false
        }
        // else if (!opened && !_isspace) {
        else if (!opened) {
            *(html + (idx++)) = *(result + i);
        }
    }
    *(html + idx) = '\0';
}
