#include <external.h> 
#include <caneka.h> 

static word betweenDef[] = {
    PAT_KO, '$', '$', patText,
    PAT_END, 0, 0
};

static word valueDef[] = {
    PAT_TERM, '{', '{', PAT_KO, '}', '}', patText,
    PAT_END, 0, 0
};

static status between(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)betweenDef, CASH_BETWEEN, -1);

    return r;
}

static status value(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)valueDef, CASH_VALUE, CASH_MARK_START);

    return r;
}

static status Cash_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    Cash *cash = as(source, TYPE_CASH);
    if(captureKey == CASH_BETWEEN){
        String_Add(cash->m, cash->s, s);
    }else if(captureKey == CASH_VALUE){
        String *value = (String *)cash->get(cash->source, (Abstract *)s);
        if(value != NULL){
            String_Add(cash->m, cash->s, value);
            cash->type.state |= SUCCESS;
        }else{
            cash->type.state &= ~SUCCESS;
            cash->type.state |= MISS;
        }
    }

    return cash->type.state;
}

String *Cash_Replace(MemCtx *m, Cash *cash, String *s){
    cash->s = String_Init(m, STRING_EXTEND);
    Roebling_Reset(m, cash->rbl, s);
    Roebling_Run(cash->rbl);

    return cash->s;
}

Cash *Cash_Make(MemCtx *m, Getter get, Abstract *source){
    Cash *cash = MemCtx_Alloc(m, sizeof(Cash));
    cash->type.of = TYPE_CASH;
    cash->m = m;

    MemHandle *mh = (MemHandle *)m;
    Span *parsers_do =  Span_From(m, 3, 
        (Abstract *)Int_Wrapped(m, CASH_MARK_START), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)between),
            (Abstract *)Do_Wrapped(mh, (DoFunc)value));

    LookupConfig config[] = {
        {CASH_MARK_START, (Abstract *)String_Make(m, bytes("CASH_START"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    String *s = String_Init(m, STRING_EXTEND);

    cash->rbl = Roebling_Make(m, TYPE_ROEBLING,
        parsers_do,
        desc,
        s,
        Cash_Capture,
        (Abstract *)cash
    ); 
    cash->get = get;
    cash->source = source;

    return cash;
}
