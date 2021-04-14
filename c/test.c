#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

void removeSpaces(char *str)
{
    // To keep track of non-space character count
    int count = 0;

    // Traverse the given string. If current character
    // is not space, then place it at index 'count++'
    for (int i = 0; *(str + i); i++)
        if (!isspace(*(str + i)))
            // printf("%c: %c\n", *(str + i), *(str + (count++)));
            *(str + (count++)) = *(str + i); // here count is
                                   // incremented
    *(str + count) = '\0';
}

int main() {

    char *str = "           \t\t\n        \v Search     Hello World";

    removeSpaces(str);

    // printf("%s\n", str);

    return 0;
}
