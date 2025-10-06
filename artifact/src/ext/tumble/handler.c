#include <external.h>
#include <caneka.h>

Abstract *Handler(MemCh *m, Handler *h, Req *req){
    Handler *next = h;
    while(next->depends != NULL){
        next = next->depends;
        if((next->type.state & (SUCCESS|ERROR)) == 0){
            h = next;
        }
    }
    return h->func(m, (Abstract *)h, (Abstract *)req);
}

Handler *Handler_Make(MemCh *m, SourceMakerFunc func, Abstract *arg){
    Handler *h = MemCh_AllocOf(m, sizeof(Handler), TYPE_HANDLER);
    h->func = func;
    h->arg = arg;
    return h;
}
