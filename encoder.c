#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    char c;
    c = fgetc(stdin);
    while(c!=EOF) {
        if((c>64) & (c<91))
            fputc(c+32, stdout);
        else
            fputc(c, stdout);
        c=fgetc(stdin);
    }
    printf("\n");
}
