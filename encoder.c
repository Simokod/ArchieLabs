#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) 
{
    // args params
    int i, j, isDebug=0, isEncrpt=0, isPlus=0, isRead=0;  
    // encrypt params
    int encIndex=2, encArgIndex;
    // reading parameters
    int readArgIndex;

    int c;
    // param check
    for(i=1;i<argc;i++){
        if(strcmp(argv[i], "-D")==0){
            if(!isDebug){
                for(j=1;j<argc;j++)
                    puts(argv[j]);
                isDebug=1;
            }
        }
        if(strncmp(argv[i], "+e", 2)==0){
            isEncrpt = 1;
            isPlus = 1;
            encArgIndex = i;
        }
        if(strncmp(argv[i], "-e", 2)==0){
            isEncrpt = 1;
            encArgIndex = i;
        }
        if(strncmp(argc[i],"-i",2)==0){
            isRead = 1;
            readArgIndex = i;
        }
    }

    c = fgetc(stdin);
    while(c!=EOF) {
        if(isDebug){
            fprintf(stderr, "0x%x\t", c);
        }

        if(isEncrpt){   // encryption
            if(isPlus){ // +e
                fputc(c + argv[encArgIndex][encIndex], stdout);
                if(isDebug){
                    fprintf(stderr, "0x%x\n", c+argv[encArgIndex][encIndex]);
                }
            }
            else {  // -e
                fputc(c - argv[encArgIndex][encIndex], stdout);
                if(isDebug)
                    fprintf(stderr, "0x%x\n", c-argv[encArgIndex][encIndex]);
            }

            ++encIndex;
            if (encIndex>strlen(argv[encArgIndex])-1)
                encIndex = 2;
            if(c == '\n'){
                encIndex=2;
                fputc('\n', stdout);
            }
        }

        else{       // no encryption
            if((c>='A') & (c<='Z')){
                if(isDebug){
                    fprintf(stderr, "0x%x\n", c+'a'-'A');
                }
                fputc(c+'a'-'A', stdout);
            }
            else {
                if(isDebug){
                    fprintf(stderr, "0x%x\n", c);
                }
                fputc(c, stdout);
            }
        }
        c=fgetc(stdin);
    }
}