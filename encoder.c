#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void encodeDebug();

int main(int argc, char **argv) 
{
    if(strcmp(argv[1], "-D")==0)
        encodeDebug();
        else {
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
}

void encodeDebug() {
    char c;
    c = fgetc(stdin);
    while(c!=EOF) {
        fprintf(stderr, "0x%x\t", c);
        if((c>64) & (c<91)){
            fprintf(stderr, "0x%x\n", c+32);
            fputc(c+32, stdout);
        }
        else{
            fprintf(stderr, "0x%x\n", c);
            fputc(c, stdout);
        }
        c=fgetc(stdin);
    }
    printf("\n");
}