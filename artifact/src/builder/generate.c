#include <external.h>

enum states {
    TEXT = 0;
    MATCH_START = 1;
    IN_MATCH = 2;
    MATCH_ENDING = 3;
}

int main(int argc, char *argv[]){
    if(argc < 4){
        printf("generate gentempl filesdir outdir - the template will such up files of the variable names\n");
        exit(1);
    }

    short state = TEXT;

    char *path = argv[1];
    char *filedir = argv[2];
    char *outdir = argv[3];

    /* open file */
    while(<file read>){
        char *start = argv[1];
        int length = strlen(start);
        char *end = start+length-1;
        char *ptr = start;

        char buff[512];
        char *b = buff;
        char *bend = buff+(512-1);

        char key[512];
        char *k = buff;
        char *kend = buff+(512-1);
        while(ptr <= end){
            char c = *ptr;
            if(state == TEXT){
                if(c == '_'){
                    state = IN_MATCH;
                }else{
                    if(b == bend){
                        /* write out */
                        b = buff;
                    }
                    *b++ = c;
                }
            }else if(state == MATCH_START){
                if(c == '_'){
                    state = IN_MATCH;
                }else{
                    state = TEXT;
                    continue;
                }
            }else if(state == IN_MATCH){
                if(c == '_'){
                    state = MATCH_ENDING;
                }else{
                    *k++ = c;
                }
            }else if(state == MATCH_ENDING){
                if(c == '_'){
                    *k++ = '\0';
                    /* write buff out */
                    b = buff;
                    /* read file */
                    /* write file */
                    k = key;
                    state = TEXT;
                }else{
                    printf("error match var needs to end in __");
                    exit(13);
                }
            }
            ptr++;
        }
    }
    printf("generated %s\n", args[1]);
    exit(0);
}
