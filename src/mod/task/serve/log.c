#include <external.h>
#include <caneka.h>


void Log_Flat(MemCh *m, Buff *out, Str *prefix, Table *data, Str *format){
    Buff *bf = Buff_Make(m, ZERO);

    if(format != NULL){
        Buff_Add(bf, format);
    }

    if(prefix != NULL){
        Buff_Add(bf, prefix);
    }

    Iter it;
    Iter_Init(&it, data);
    void *ar[3];
    ar[2] = NULL;
    char *fmtN = ", $:$";
    char *fmtFirst = "$:$";
    char *fmt = fmtFirst;
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            ar[0] = h->key;
            ar[1] = h->value;
            Fmt(bf, fmt, ar);
            if(fmt == fmtFirst){
                fmt = fmtN;
            }
        }
    }

    if(format != NULL){
        Fmt(bf, "^0.\n", NULL);
    }else{
        Buff_Add(bf, K(m, "\n"));
    }

    Buff_Pipe(out, bf);
}
