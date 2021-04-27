#include <stdio.h>
#include <stdlib.h>


int main() {

    FILE *f = fopen("a.out", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    // string[fsize] = 0;
    *(string + fsize) = 0;

    while (*string) {
        printf("%x\n", *string++);
    }

    printf("%x\n", string);

    printf("%ld\n", fsize);

    return 0;
}
