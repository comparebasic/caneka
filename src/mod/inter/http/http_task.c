#include <external.h>
#include <caneka.h>

status HttpTask_InitResponse(Task *tsk, void *arg, void *source){
    DebugStack_Push(tsk, tsk->type.of);
    status r = READY;

    ProtoCtx *proto = HttpProto_Make(tsk->m);

    TcpCtx *tcp = (TcpCtx *)Ifc(tsk->m, source, TYPE_TCP_CTX);
    if(tcp->defaultData != NULL){
        HttpCtx *http = (HttpCtx*)proto->ctx;
        SourceFunc func = (SourceFunc)tcp->defaultData;
        func(tsk->m, http->data, tcp);
    }

    tsk->data = (Abstract *)proto;
    tsk->source = (Abstract *)tcp;


    DebugStack_Pop();
    return r;
}

status HttpTask_AddRecieve(Task *tsk, void *arg, void *source){
    DebugStack_Push(tsk, tsk->type.of);
    ProtoCtx *proto = (ProtoCtx *)Ifc(tsk->m, tsk->data, TYPE_PROTO_CTX);
    Cursor *curs = Cursor_Make(tsk->m, proto->in->v);
    Roebling *rbl = HttpRbl_Make(tsk->m, curs, proto);
    status r = Task_AddStep(tsk, TcpTask_ReadToRbl, rbl, source, STEP_IO_IN);

    TcpTask_ExpectRecv(NULL, tsk);

    DebugStack_Pop();
    return r;
}
