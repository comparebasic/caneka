#include <external.h>
#include <caneka.h>

status Uri_Print(Buff *bf, void *a, cls type, word flags){
    Uri *uri = (Uri *)a;
    void *ar[] = {
        Type_StateVec(bf->m, uri->type.of, uri->type.state),
        uri->v,
        uri->proto,
        uri->host,
        uri->path,
        uri->queryIt.p,
        NULL
    };
    Fmt(bf, "Uri<@ @ proto:@ host:@ path:@ query:@>", ar);

    return SUCCESS;
}

status Uri_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_URI, (void *)Uri_Print);
    return r;
}
