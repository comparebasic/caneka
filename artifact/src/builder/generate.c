#include <external.h>
#include <caneka.h>
#include <builder.h>

enum states {
    TEXT = 1 << 8,
    MATCH_START = 1 << 9,
    IN_MATCH = 1 << 10,
    MATCH_ENDING = 1 << 11,
};

status Generate(MemCh *m, Str *path, Str* filedir, Str *outdir){

    status state = TEXT;

    /* open file */
    i32 fd = open(Str_Cstr(m, path), O_RDONLY);
    if(fd < 0){
        Abstract *args[] = {
            (Abstract *)path,
            NULL
        };
        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
            "Generate unable to open template path: $", args);
        return ERROR;
    }

    char in[BUILDER_READ_SIZE];
    ssize_t length = 0;
    ssize_t seglen = 0;
    do {
        length = read(fd, in, BUILDER_READ_SIZE);

        char *start = in;
        int length = strlen(start);
        char *end = start+length-1;
        char *ptr = start;

        char buff[BUILDER_READ_SIZE];
        char *b = buff;
        char *bend = buff+(BUILDER_READ_SIZE-1);

        char key[BUILDER_READ_SIZE];
        char *k = buff;
        char *kend = buff+(BUILDER_READ_SIZE-1);

        while(ptr <= end){
            char c = *ptr;
            if(state == TEXT){
                if(c == '_'){
                    state = IN_MATCH;
                }else{
                    if(b == bend){
                        seglen = end - ptr;
                        write(1, "text: ", strlen("text: "));
                        write(1, b, seglen);
                        write(1, "\n\n", 2);
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
                    seglen = end - ptr;
                    if(seglen > 0){
                        write(1, "text: ", strlen("text: "));
                        write(1, b, seglen);
                        write(1, "\n\n", 2);
                    }
                    b = buff;

                    seglen = kend - k;
                    if(seglen > 0){
                        write(1, "filename: ", strlen("filename: "));
                        write(1, k, seglen);
                        write(1, "\n\n", 2);
                    }

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
    }while(length > 0);
    if(length < 0){
        Abstract *args[] = {
            (Abstract *)path,
            (Abstract *)Str_CstrRef(m,  strerror(errno)),
            NULL
        };
        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
            "Generate read error on file $: $", args);
        return ERROR;
    }

    state |= SUCCESS;
    return state;
}
