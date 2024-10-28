#include <external.h>
#include <caneka.h>

/* token patCharDefs */
static word labelDef[] = {
    PAT_KO|PAT_NO_CAPTURE, ':',':', patText,
    PAT_END, 0, 0
};
static word tokenNameDef[] = {
    PAT_KO, '/','/', patText,
    PAT_END, 0, 0
};

static word lengthDef[] = {
    PAT_NO_CAPTURE|PAT_TERM, '/','/',PAT_MANY|PAT_TERM,'0', '9',PAT_NO_CAPTURE|PAT_TERM, '=', '=',
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
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)labelNameDef, OSET_TYPE, OSET_MARK_LENGTH);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenNameDef, OSET_TYPE, -1);
    return r;
}

static status token(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenNameDef, OSET_TYPE, -1);
    return r;
}

static status defineLength(Roebling *rbl){
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lengthDef, OSET_LENGTH, -1);
    return r;
}

static status value(Roebling *rbl){
    Roebling_ResetPatterns(rbl);

    Match *mt = Span_ReserveNext(rbl->matches);
    Oset *oset = (Oset *)as(rbl->source, TYPE_OSET);

    status r = READY;
    r |= Match_SetPattern(mt, valueDef);
    mt->captureKey = OSET_VALUE;
    mt->remaining = oset->remaining;

    return r;
}

static status OsetParser_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    Oset *oset = (Oset *)as(source, TYPE_OSET);

    oset->remaining = 0;
    if(captureKey == OSET_LENGTH){
       oset->remaining = Int_FromString(s); 
    }

    status r = SUCCESS;
    return r;
}


Roebling *OsetParser_Make(MemCtx *m, String *s, Abstract *source){
    MemHandle *mh = (MemHandle *)m;
    Span *parsers_do =  Span_From(m, 4, 
        (Abstract *)Int_Wrapped(m, OSET_MARK_START), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)start),
            (Abstract *)Do_Wrapped(mh, (DoFunc)token),
        (Abstract *)Int_Wrapped(m, OSET_MARK_LENGTH), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)defineLength),
            (Abstract *)Do_Wrapped(mh, (DoFunc)valueDef));

    LookupConfig config[] = {
        {OSET_START, (Abstract *)String_Make(m, bytes("OSET_START"))},
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
