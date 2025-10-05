#include <external.h>
#include <caneka.h>

status Serve_CloseReq(Serve *sctx, i32 idx){
    Req *req = Span_Get(sctx->q->itemsIt.p, idx);
    Queue_Remove(sctx->q, idx);
    struct pollfd *pfd = *req->id;
    close(pfd->fd);
    sctx->metrics.open--;
    if((req->type.state & ERROR) != 0){
        sctx->metrics.error++;
    }else{
        sctx->metrics.served++;
    }
    if(req->m->owner == NULL || req->m->owner == (Abstract *)req){
        MemCh_Free(req->m);
    }
    return SUCCESS;
}

status Serve_Stop(Serve *sctx){
    status r = READY;
    /*
    Queue *q = &sctx->queue;
    q->type.state &= ~END;
    while((q->type.state & END) == 0 && (r & ERROR) == 0){
        QueueIdx *qidx = Queue_Next(q, NULL);
        if(qidx != NULL){
            Req *req = (Req *)qidx->item;
            sctx->active = req;
            r |= Serve_CloseReq(sctx, req, q->current.idx);
        }
    }
    close(sctx->socket_fd);
    */
    return r;
}

Serve *Serve_Make(MemCtx *m, ServeMaker req, ServeMaker handlers){
    Serve *sctx = (Serve *)MemCtx_Alloc(m, sizeof(Serve)); 
    sctx->type.of = TYPE_SERVECTX;
    sctx->m = m;
    sctx->q = Queue_Make(m);
    sctx->socket_fd = -1;
    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Fds, ZERO);
    Queue_AddHandler(q, crit);
    return sctx;
}
