#include <caneka.h>
#include <tests.h>

status CharPtr_ToHelp(MemCh *m, Str *name, Table *resolve, int argc, char **argv){
    Abstract *args[] = {
        (Abstract *)name,
        (Abstract *)resolve,
        NULL
    };
    Out("$ @\n", NULL);
    exit(1);
}

status CharPtr_ToTbl(MemCh *m, Table *resolve, int argc, char **argv, Table *dest){
    status r = READY;
    Str *key = NULL;
    Iter it;
    Iter_Init(&it, dest);
    if(argc < 2){
        return NOOP;
    }else{
        argv++;
        for(i32 i = 1; i < argc; i++, argv++){
            Str *s = Str_CstrRef(m, *argv);
            if(it.metrics.set != -1){
                 printf("Setting value upper for %s\n", s->bytes);
                 Table_SetValue(&it, (Abstract *)s);
                 key = NULL;
                 r |= SUCCESS;
            }
            if(s->bytes[0] == '-'){
                 Str_Incr(s, 1);
                 if(Table_Get(resolve, (Abstract *)s) != NULL){
                    key = s;
                    printf("SetKey\n");
                    Table_SetKey(&it, (Abstract *)key);
                    printf("Setting key %s %d\n", s->bytes, it.metrics.set);
                 }else{
                    Abstract *args[] = {
                        (Abstract *)s,
                        NULL
                    };
                    Error(ErrStream->m, (Abstract *)resolve, 
                        FUNCNAME, FILENAME, LINENUMBER, "Unable to find resolve for arg @",
                        args);
                 }
            }else{
                if(it.metrics.set == -1){
                    printf("Setting key for %s\n", s->bytes);
                    Table_SetKey(&it, (Abstract *)I32_Wrapped(m, i));
                }
                printf("Setting value for %s\n", s->bytes);
                Table_SetValue(&it, (Abstract *)s);
            }
        }
    }
    return r;
}
