#include <external.h>
#include <caneka.h>

Abstract *Handler_Add(MemCh *m, Handler *chain, HandlerFunc func, Abstract *arg, Abstract *source){
    Handler *tail = Handler_Tail(chain);
    tail->depends = Handler_Make(m, func, arg, source);
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

Handler *Handler_Make(MemCh *m, HandlerFunc func, Abstract *arg, Abstract *source){
    Handler *h = MemCh_AllocOf(m, sizeof(Handler), TYPE_HANDLER);
    h->func = func;
    h->arg = arg;
    h->source = source;
    return h;
}
