#include <external.h>
#include <caneka.h>

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    return r;
}

status CnkRoebling_AddParsers(MemCtx *m, Span *parsers, Lookup *marks){
    Span *additions = Span_Make(m, TYPE_SPAN);
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_START));
    Span_Add(additions, (Abstract *)Do_Wrapped(m, (DoFunc)start));

    LookupConfig config[] = {
        {CNK_LANG_RBL_START, (Abstract *)String_Make(m, bytes("RBL_START"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    return Roebling_AddParsers(m, parsers, marks, additions,  desc);
}
