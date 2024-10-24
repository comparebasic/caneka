#include <external.h>
#include <caneka.h>

/* example
oset/:s/5=title;kv/:=s/3=hey;a:b64/12=XafjkacC;;;

oset
    string = title
    key value = 
        string = hey -> address = (base64) XafjkacC
*/



Chain *OsetChain = NULL;

static status populateOset(MemCtx *m, Lookup *lk){
    status r = READY;
    /*
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Abstract_Print);
    r |= Lookup_Add(m, lk, TYPE_MATCH, (void *)Match_Print);
    r |= Lookup_Add(m, lk, TYPE_STRINGMATCH, (void *)StringMatch_Print);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)PatCharDef_Print);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_STRING_CHAIN, (void *)String_Print);
    r |= Lookup_Add(m, lk, TYPE_STRING_FIXED, (void *)StringFixed_Print);
    r |= Lookup_Add(m, lk, TYPE_SCURSOR, (void *)SCursor_Print);
    r |= Lookup_Add(m, lk, TYPE_RANGE, (void *)Range_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_MINI_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_ROEBLING, (void *)Roebling_Print);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Print);
    r |= Lookup_Add(m, lk, TYPE_SINGLE, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_RBL_MARK, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_UTIL, (void *)WrappedUtil_Print);
    r |= Lookup_Add(m, lk, TYPE_MESS, (void *)Mess_Print);
    r |= Lookup_Add(m, lk, TYPE_LOOKUP, (void *)Lookup_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN_QUERY, (void *)SpanQuery_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN_STATE, (void *)SpanState_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE, (void *)Queue_Print);
    */
    return r;
}


status Oset_Init(MemCtx *m){
    if(OsetChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateOset, NULL);
        OsetChain = Chain_Make(m, funcs);
        return SUCCESS;
    }
    return NOOP;
}

String *Oset_To(MemCtx *m, Abstract *a){
    Maker func = (Maker)Chain_Get(OsetChain, a->type.of);
    if(func != NULL){
        return func(m, a);
    }else{
        Fatal("Uknown type for parsing Oset", TYPE_ABSTRACT);
    }
}

Roebling *OsetParser_Make(MemCtx *m, String *Source){
    MemHandle *mh = (MemHandle *)m;

    Span *parsers_do =  Span_From(m, 1, 
        (Abstract *)Int_Wrapped(m, OSET_START));

    LookupConfig config[] = {
        {OSET_START, (Abstract *)String_Make(m, bytes("OSET_START"))},
        {OSET_KEY, (Abstract *)String_Make(m, bytes("OSET_KEY"))},
        {OSET_LENGTH, (Abstract *)String_Make(m, bytes("OSET_LENGTH"))},
        {OSET_OPTS, (Abstract *)String_Make(m, bytes("OSET_OPTS"))},
        {OSET_VALUE, (Abstract *)String_Make(m, bytes("OSET_VALUE"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    return Roebling_Make(m, TYPE_ROEBLING, parsers_do, desc, String_Init(m, STRING_EXTEND), Oset_Capture, (Abstract *)xml->ctx); 

    return xml;
}
