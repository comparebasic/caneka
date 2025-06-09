#include <external.h>
#include <caneka.h>

char *ProcIoSet_FlagsToChars(word flags){
    if(flags & PROCIO_INREQ){
        return "PROCIO_INREQ";
    }else if(flags & PROCIO_OUTREQ){
        return "PROCIO_OUTREQ";
    }else if(flags & PROCIO_ERRREQ){
        return "PROCIO_ERRREQ";
    }else{
        return "UNKNOWN";
    }
}

ProcIoSet *ProcIoSet_Make(MemCtx *m){
    ProcIoSet *set = MemCtx_Alloc(m, sizeof(ProcIoSet));
    set->type.of = TYPE_PROC_IO_SET;
    set->cmds = Span_Make(m);
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

ProcIoSet *ProcIoSet_SubProc(Serve *sctx, Span *cmds){
    MemCtx *m = MemCtx_Make();
    ProcIoSet *set = ProcIoSet_Make(m);
    ProcDets_Init(&set->pd);
    set->pd.type.state |= PROCDETS_ASYNC;
    m->owner = (Abstract *)set;
    set->cmds = cmds;

    set->type.state |= SubCall(sctx->m, cmds, &set->pd);
    if(set->type.state & SUCCESS){
        Req *req = NULL;
        req = Serve_AddFd(m, sctx, set->pd.inFd, PROCIO_INREQ);
        ProcIoSet_Add(set, req);

        req = Serve_AddFd(m, sctx, set->pd.outFd, PROCIO_OUTREQ);
        ProcIoSet_Add(set, req);

        req = Serve_AddFd(m, sctx, set->pd.errFd, PROCIO_ERRREQ);
        ProcIoSet_Add(set, req);
    }

    return set;
}
