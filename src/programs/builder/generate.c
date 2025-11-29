#include <external.h>
#include <caneka.h>
#include "module.h"

static status out(i32 fd, char *buff, char **b){
    if(*b != buff){
        write(fd, buff, *b - buff);
        *b = buff;
        return SUCCESS;
    }
    return NOOP;
}

static status outfile(i32 ofd, char *vpath){
    i32 fd = open(vpath, O_RDONLY);
    void *args[2];
    if(fd < 0){
        args[0] = Str_CstrRef(OutStream->m, vpath);
        args[1] = NULL;
        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
            "Generate unable to open variable path: $", args);
        return ERROR;
    }

    char in[BUILDER_READ_SIZE];
    ssize_t length = 0;

    do {
        length = read(fd, in, BUILDER_READ_SIZE);
        if(length <= 0){
            break;
        }
        write(ofd, in, length);
    }while(length > 0);

    if(length < 0){
        args[0] = Str_CstrRef(OutStream->m, vpath);
        args[1] = NULL;
        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
            "Generate writing to open variable path: $", args);
        return ERROR;
    }

    return SUCCESS;
}

status Generate(MemCh *m, Str *path, Str *key, char* varpaths[], Str *outpath){
    status state = READY;

    /* open file */
    i32 fd = open(Str_Cstr(m, path), O_RDONLY);
    i32 mode = 0644;
    i32 ofd = open(Str_Cstr(m, outpath), O_WRONLY|O_CREAT, mode);

    if(fd < 0 || ofd < 0){
        void *args[] = {
            path,
            outpath,
            Str_CstrRef(ErrStream->m, strerror(errno)),
            NULL
        };
        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
            "Generate unable to open template path $ -> $: $", args);
        return ERROR;
    }

    char in[BUILDER_READ_SIZE];
    ssize_t length = 0;
    ssize_t seglen = 0;
    char buff[BUILDER_READ_SIZE];
    char *b = buff;
    char *bend = buff+(BUILDER_READ_SIZE-1);
    char shelf[BUILDER_READ_SIZE];
    char *sh = shelf;
    char *shend = shelf+(BUILDER_READ_SIZE-1);

    i32 varIdx = 0;
    i16 keyIdx = 0;
    do {
        length = read(fd, in, BUILDER_READ_SIZE);

        if(length <= 0){
            break;
        }

        char *start = in;
        char *end = start+length-1;
        char *ptr = start;

        while(ptr <= end){
            char c = *ptr;
            char k = key->bytes[keyIdx];
            if(c == k){
                keyIdx++;
                *sh++ = c;
                if(keyIdx == key->length){
                    out(ofd, buff, &b);
                    char *vpath = varpaths[varIdx];
                    if(vpath == NULL || (outfile(ofd, vpath) & ERROR)){
                        void *args[] = {
                            Str_CstrRef(ErrStream->m, vpath),
                            Str_CstrRef(ErrStream->m, strerror(errno)),
                            NULL
                        };
                        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
                            "Generate unable to read variable file $: $", args);
                        return ERROR;
                    }
                    varIdx++;
                    sh = shelf;
                    keyIdx = 0;
                }else{
                    if(sh == shend){
                        out(ofd, shelf, &sh);
                    }
                }
            }else{
                keyIdx = 0;
                if(sh != shelf){
                    out(ofd, buff, &b);
                    out(ofd, shelf, &sh);
                }else if(b == bend){
                    out(ofd, buff, &b);
                }
                *b++ = c;
            }
            ptr++;
        }

    } while(length > 0);

    out(ofd, buff, &b);

    if(length < 0){
        void *args[] = {
            path,
            Str_CstrRef(m,  strerror(errno)),
            NULL
        };
        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
            "Generate read error on file $: $", args);
        return ERROR;
    }

    state |= (close(fd) == 0) ? SUCCESS : ERROR;
    state |= (close(ofd) == 0) ? SUCCESS : ERROR;

    return state;
}
