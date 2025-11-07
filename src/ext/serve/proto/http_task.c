#include <external.h>
#include <caneka.h>

status HttpTask_InitResponse(Task *tsk, Abstract *arg, Abstract *source){
    DebugStack_Push(tsk, tsk->type.of);
    status r = READY;
    if(tsk->data == NULL){
        tsk->data = (Abstract *)HttpProto_Make(tsk->m);
    }
    tsk->source = source;

    DebugStack_Pop();
    return r;
}

status HttpTask_AddRecieve(Task *tsk, Abstract *arg, Abstract *source){
    DebugStack_Push(tsk, tsk->type.of);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    Cursor *curs = Cursor_Make(tsk->m, proto->in->v);
    Roebling *rbl = HttpRbl_Make(tsk->m, curs, (Abstract *)proto);
    status r = Task_AddStep(tsk, TcpTask_ReadToRbl, (Abstract *)rbl, source, STEP_IO_IN);

    tsk->type.state |= TcpTask_ExpectRecv(NULL, tsk);

    DebugStack_Pop();
    return r;
}
