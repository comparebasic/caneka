#include <external.h>
#include <caneka.h>

/* token patCharDefs */
static word labelDef[] = {
    PAT_KO|PAT_CONSUME, ':',':', patText,
    PAT_END, 0, 0
};

static word tokenNameDef[] = {
    PAT_KO, '/','/', patText,
    PAT_END, 0, 0
};

static word lengthDef[] = {
    PAT_MANY|PAT_TERM,'0', '9',PAT_NO_CAPTURE|PAT_TERM, '=', '=',
    PAT_END, 0, 0
};

static word valueDef[] = {
    PAT_COUNT|PAT_TERM, 0, 255,
    PAT_END, 0, 0
};

static word sepDef[] = {
    PAT_TERM, ';',';',
    PAT_END, 0, 0
};

/* match setups */
static status start(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)labelDef, OSET_KEY, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenNameDef, OSET_TOKEN, OSET_MARK_LENGTH);
    return r;
}

static status token(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenNameDef, OSET_TOKEN, -1);
    return r;
}

static status defineLength(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lengthDef, OSET_LENGTH, -1);
    return r;
}

static status value(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    Match *mt = Span_ReserveNext(rbl->matches);
    Oset *oset = (Oset *)as(rbl->source, TYPE_OSET);

    r |= Match_SetPattern(mt, (PatCharDef *)valueDef);
    mt->captureKey = OSET_VALUE;
    mt->remaining = oset->remaining;

    return r;
}

static status Oset_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    Oset *oset = (Oset *)as(source, TYPE_OSET);
    if(captureKey == OSET_LENGTH){
        oset->remaining = Int_FromString(s);
    }else if(captureKey == OSET_KEY){
        oset->key = s;
    }else if(captureKey == OSET_TOKEN){
        OsetDef *odef = TableChain_Get(oset->byName, s);
        if(odef == NULL){
            Fatal("Error: type not found\n", TYPE_OSET);
        }
        oset->odef = odef;
    }else if(captureKey == OSET_VALUE){
        oset->content = s;
    }
    return SUCCESS;
}

Roebling *OsetParser_Make(MemCtx *m, String *s, Abstract *source){
    MemHandle *mh = (MemHandle *)m;
    Span *parsers_do =  Span_From(m, 6, 
        (Abstract *)Int_Wrapped(m, OSET_MARK_START), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)start),
            (Abstract *)Do_Wrapped(mh, (DoFunc)token),
        (Abstract *)Int_Wrapped(m, OSET_MARK_LENGTH), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)defineLength),
            (Abstract *)Do_Wrapped(mh, (DoFunc)value));

    LookupConfig config[] = {
        {OSET_MARK_START, (Abstract *)String_Make(m, bytes("OSET_START"))},
        {OSET_MARK_LENGTH, (Abstract *)String_Make(m, bytes("OSET_LENGTH"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    if(s == NULL){
        s = String_Init(m, STRING_EXTEND);
    }

    return Roebling_Make(m, TYPE_ROEBLING,
        parsers_do,
        desc,
        s,
        Oset_Capture,
        source 
    ); 
}
