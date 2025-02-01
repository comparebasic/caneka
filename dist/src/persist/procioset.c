#include <external.h>
#include <caneka.h>

ProcIoSet *ProcIoSet_Make(MemCtx *m){
    ProcIoSet *set = MemCtx_Alloc(m, sizeof(ProcIoSet));
    set->type.of = TYPE_PROC_IO_SET;
    set->cmds = Span_Make(m, TYPE_SPAN);
    return set;
}

status ProcIoSet_Add(ProcIoSet *set, Req *req){
    if((req->type.state & PROCIO_INREQ) != 0){
        set->in = req;
        req->in.rbl->source = (Abstract *)set;
        return SUCCESS;
    }else if((req->type.state & PROCIO_OUTREQ) != 0){
        set->out = req;
        req->in.rbl->source = (Abstract *)set;
        return SUCCESS;
    }else if((req->type.state & PROCIO_ERRREQ) != 0){
        set->err = req;
        req->in.rbl->source = (Abstract *)set;
        return SUCCESS;
    }

    return NOOP;
}
