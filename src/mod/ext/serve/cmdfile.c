#include <external.h>
#include <caneka.h>

HandlerDef *CmdFile_DefMake(MemCh *m, Span *steps, Node *ext, Abstract *key, Node *config){
    Buff *bf = Ifc(m, key, TYPE_BUFF);

    HandlerDef *def = HandlerDef_Make(m);

    def->extra = (SourceMakerFunc)NULL;
    def->finalize = (ReqFunc)NULL;
    def->setup = (DoFunc)NULL;
    def->log.open = (ReqFunc)NULL;
    def->log.final = (ReqFunc)NULL;

    def->route = Span_Make(m);
    /*
    Span_Add(def->route, Func_Wrapped(m, HttpReq_Write, SEND_FLAGS));
    Span_AddSpanRev(def->route, steps);
    Span_Add(def->route, Func_Wrapped(m, HttpReq_ReadToRbl, RECV_FLAGS));

    */
    def->ext = ext;

    return def;
}
