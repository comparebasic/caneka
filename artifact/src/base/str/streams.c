#include <external.h>
#include <caneka.h>

StrVec *StrVec_FromBytes(MemCh *m, byte *b, i32 length){
    Stream sm;
    memset(&sm, 0, sizeof(Stream));
    StrVec *v = StrVec_Make(m);
    Stream_SetupMakeStrVec(m, &sm, v);
    Stream_Bytes(&sm, b, length);
    return v;
}

i64 Stream_ToFd(Stream *sm, byte *b, i32 length){
    ssize_t l = write(sm->fd, b, (size_t)length);
    if(l < 0){
        Abstract *args[2] = {
            (Abstract *)Str_CstrRef(ErrStream->m, strerror(errno)),
            NULL
        };
        Error(ErrStream->m, (Abstract *)sm, FUNCNAME, FILENAME, LINENUMBER,
            "Error wrtiing to file: $", args);
        return 0;
    }
    return l;
}

i64 Stream_ToStrVec(Stream *sm, byte *b, i32 length){
    i32 sz = STR_DEFAULT;
    i64 taken = 0;
    i64 offset = 0;
    i64 total = 0;
    Cursor *curs = sm->dest.curs;
    StrVec *v = sm->dest.curs->v;
    while(length > 0 ){
        Str *s = Iter_Current(&curs->it);
        if(s == NULL || s->length == s->alloc){
            s = Str_Make(sm->m, sz);
            Iter_Add(&curs->it, s);
        }

        taken = Str_Add(s, b+offset, length);

        v->total += taken;
        total += taken;
        length -= taken;
        offset += taken;
    }
    return total;
}
