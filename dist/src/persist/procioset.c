#include <external.h>
#include <caneka.h>

ProcIoSet *ProcIoSet_Make(MemCtx *m){
    ProcIoSet *set = MemCtx_Alloc(m, sizeof(ProcIoSet));
    set->type.of = TYPE_PROC_IO_SET;
    set->cmds = Span_Make(m, TYPE_SPAN);
    return set;
}

status ProcIoSet_FlagAll(ProcIoSet *pio, word set, word unset){
    status r = READY;
    if(pio->in != NULL){
        pio->in->type.state |= set;
        pio->in->type.state &= ~unset;
        r |= SUCCESS;
    }
    if(pio->out != NULL){
        pio->out->type.state |= set;
        pio->out->type.state &= ~unset;
        r |= SUCCESS;
    }
    if(pio->err != NULL){
        pio->err->type.state |= set;
        pio->err->type.state &= ~unset;
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }
    
    return r;
}

status ProcIoSet_Add(ProcIoSet *set, Req *req){
    SubProto *proto = (SubProto *)as(req->proto, TYPE_SUB_PROTO);
    proto->procio = set;
    if((req->type.state & PROCIO_INREQ) != 0){
        set->in = req;
        return SUCCESS;
    }else if((req->type.state & PROCIO_OUTREQ) != 0){
        set->out = req;
        return SUCCESS;
    }else if((req->type.state & PROCIO_ERRREQ) != 0){
        set->err = req;
        return SUCCESS;
    }

    return NOOP;
}
