#include <external.h>
#include <caneka.h>

Abstract *Handler_Add(MemCh *m, Handler *chain, SourecMaker func, Abstract *arg, Abstract *source){
    Handler *tail = Handler_Tail(chain);
    tail->depends = func(m, arg, source);
    return tail->depends;
}

Abstract *Handler_Tail(Handler *chain){
    while(chain->depends != NULL){
        chain = chain->depends;
    }
    return chain;
}

Abstract *Handler_Get(MemCh *m, Handler *h, Req *req){
    Handler *next = h;
    while(next->depends != NULL){
        next = next->depends;
        if((next->type.state & (SUCCESS|ERROR)) == 0){
            h = next;
        }
    }
    return h;
}

Handler *Handler_Make(MemCh *m, SourceMakerFunc func, Abstract *arg){
    Handler *h = MemCh_AllocOf(m, sizeof(Handler), TYPE_HANDLER);
    h->func = func;
    h->arg = arg;
    return h;
}
