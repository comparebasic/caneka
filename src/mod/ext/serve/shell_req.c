#include <external.h>
#include <caneka.h>

void *ShellReq_SourceMake(MemCh *m, Abstract *key, HandlerDef *def){
    ShellReq *sreq = MemCh_AllocOf(m, sizeof(ShellReq), TYPE_SHELL_REQ);
    sreq->type.of = TYPE_SHELL_REQ;
    sreq->m = m;

    sreq->in = Buff_Make(m, BUFF_UNBUFFERED);
    Buff_SetFd(sreq->in, 0);
    sreq->out = Buff_Make(m, BUFF_UNBUFFERED);
    Buff_SetFd(sreq->out, 1);
    sreq->err = Buff_Make(m, BUFF_UNBUFFERED);
    Buff_SetFd(sreq->err, 2);

    sreq->shelf = StrVec_Make(m);
    sreq->env = Table_Make(m);
    sreq->args = Table_Make(m);
    sreq->procs = Table_Make(m);

    return sreq;
}

HandlerDef *ShellReq_DefMake(MemCh *m,
        Span *steps, Node *ext, Abstract *key, Node *config){
    HostEnt *ent = Ifc(m, key, TYPE_HOST_ENT);

    HandlerDef *def = HandlerDef_Make(m);

    def->ent = ent;
    def->extra = (SourceMakerFunc)ShellReq_SourceMake;
    def->finalize = (ReqFunc) HttpReq_Finalize;
    def->setup = (DoFunc) HttpReq_Setup;
    def->log.open = (ReqFunc) HttpReq_LogOpen;
    def->log.final = (ReqFunc) HttpReq_LogFinalized;

    def->route = Span_Make(m);
    Span_Add(def->route, Func_Wrapped(m, HttpReq_Write, SEND_FLAGS));
    Span_AddSpanRev(def->route, steps);
    Span_Add(def->route, Func_Wrapped(m, HttpReq_ReadToRbl, RECV_FLAGS));

    def->ext = ext;

    return def;
}
