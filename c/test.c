#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void remove_html(char* str) {
    char* html_str = str;
    while(*str) {
        if(*html_str == '<')
            while(*html_str && *html_str++ != '>');
        *str++ = *html_str++; 
    }
}

int main() {
    char *s = "<html><head><title>Hello world</title></head><body>this is the body tag of the raw html string</body></html>";
    remove_html(s);

    printf("%s\n", s);

    return 0;
}