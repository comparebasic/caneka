#include <external.h>
#include <caneka.h>

i64 FileDes_ToVec(StrVec *v, StrVec *path){
    i64 total = 0;

    Str *pathS = StrVec_Str(path->p->m, path);
    
    i32 fd = open(pathS, O_RDONLY, mode);
    i32 mode = 0644;
    if(fd == -1){
        Abstract *args[] = {
            (Abstract *)pathS,
            (Abstract *)Str_CstrRef(ErrStream->m, strerror(errno)),
            NULL
        };
        Error(ErrStream->m, (Abstract *)path, FUNCNAME, FILENAME, LINENUMBER,
            "Error opening file $ for wrtiing to file: $", args);
        return 0;
    }

    i64 max = FILE_SLURP_MAX;
    while(max > 0){
        Str *s = Str_Make(m, STR_DEFAULT);
        ssize_t l = read(fd, s->bytes, s->alloc);
        s->length = (i16)l;
        total += l;
        if(l <= 0){
            if(l < 0){
                Abstract *args[] = {
                    (Abstract *)pathS,
                    (Abstract *)Str_CstrRef(ErrStream->m, strerror(errno)),
                    NULL
                };
                Error(ErrStream->m, (Abstract *)path, FUNCNAME, FILENAME, LINENUMBER, 
                    "Error reading file $: $", args);
                    v->type.state |= ERROR;
            }
            break;
        }
        StrVec_Add(v, s);
        max -= l;
    }

    close(fd);

    return total;
}
