#include <external.h>
#include <caneka.h>

ProcIoSet *ProcIoSet_Make(MemCtx *m){
    ProcIoSet *set = MemCtx_Alloc(m, sizeof(ProcIoSet));
    set->type.of = TYPE_PROC_IO_SET;
    set->cmds = Span_Make(m, TYPE_SPAN);
    return set;
}

status ProcIoSet_SegFlags(ProcIoSet *pio, Serve *sctx, status flags){
    pio->in->type.state |= flags;
    pio->out->type.state |= flags;
    pio->err->type.state |= flags;
    Serve_ClobberFlags(sctx, pio->in, flags);
    Serve_ClobberFlags(sctx, pio->out, flags);
    Serve_ClobberFlags(sctx, pio->err, flags);
    return SUCCESS;
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
