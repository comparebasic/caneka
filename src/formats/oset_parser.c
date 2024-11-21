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

static word startListDef[] = {
    PAT_TERM,'(', '(',
    PAT_END, 0, 0
};

static word startArrayDef[] = {
    PAT_TERM,'[', '[',
    PAT_END, 0, 0
};

static word startTableDef[] = {
    PAT_TERM,'{', '{',
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

static word endListDef[] = {
    PAT_TERM, ')',')',
    PAT_END, 0, 0
};

static word endArrayDef[] = {
    PAT_TERM, ']',']',
    PAT_END, 0, 0
};

static word endTableDef[] = {
    PAT_TERM, '}','}',
    PAT_END, 0, 0
};

/* match setups */
static status start(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)endListDef, OSET_CLOSE_LIST, OSET_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)endArrayDef, OSET_CLOSE_ARRAY, OSET_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)endTableDef, OSET_CLOSE_TABLE, OSET_MARK_START);

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

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)startListDef, OSET_LENGTH_LIST, OSET_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)startArrayDef, OSET_LENGTH_ARRAY, OSET_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)startTableDef, OSET_LENGTH_TABLE, OSET_MARK_START);

    Match *mt = Span_ReserveNext(rbl->matches);
    Oset *oset = (Oset *)as(rbl->source, TYPE_OSET);

    r |= Match_SetPattern(mt, (PatCharDef *)valueDef);
    mt->captureKey = OSET_VALUE;
    mt->remaining = oset->item->remaining;

    return r;
}

static status sep(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)sepDef, OSET_SEP, OSET_MARK_START);
    return r;
}

static status Oset_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    Oset *oset = (Oset *)as(source, TYPE_OSET);
    if(oset->item == NULL){
        oset->item = OsetItem_Make(oset->m, oset);
    }
    if(captureKey == OSET_KEY){
        oset->item->key = s;
    }else if(captureKey == OSET_TOKEN){
        OsetDef *odef = TableChain_Get(oset->byName, s);
        if(odef == NULL){
            Fatal("Error: type not found\n", TYPE_OSET);
        }
        oset->item->odef = odef;
    }else if(captureKey == OSET_LENGTH){
        oset->item->remaining = Int_FromString(s);
    }else if(captureKey == OSET_LENGTH_ARRAY){
        if(oset->item->odef->typeOf != TYPE_SPAN){
            return ERROR;
        }
        oset->item->value = (Abstract *)Span_Make(oset->m, TYPE_SPAN);
        oset->type.state |= PARENT_TYPE_ARRAY;

        OsetItem *item = OsetItem_Make(oset->m, oset);
        item->parent = oset->item;
        oset->item = item;
    }else if(captureKey == OSET_LENGTH_LIST){
        if(oset->item->odef->typeOf != TYPE_SPAN){
            return ERROR;
        }
        oset->item->value = (Abstract *)Span_Make(oset->m, TYPE_SPAN);
        
        OsetItem *item = OsetItem_Make(oset->m, oset);
        item->parent = oset->item;
        item->type.state |= PARENT_TYPE_ARRAY;
        oset->item = item;
    }else if(captureKey == OSET_LENGTH_TABLE){
        if(oset->item->odef->typeOf != TYPE_TABLE){
            return ERROR;
        }
        oset->item->value = (Abstract *)Span_Make(oset->m, TYPE_TABLE);
        
        OsetItem *item = OsetItem_Make(oset->m, oset);
        item->type.state |= PARENT_TYPE_TABLE;
        item->parent = oset->item;
        oset->item = item;

    }else if(captureKey == OSET_CLOSE_ARRAY || captureKey == OSET_CLOSE_LIST || captureKey == OSET_CLOSE_TABLE){
        oset->item = oset->item->parent;
    }else if(captureKey == OSET_CLOSE_TABLE){
        oset->item = oset->item->parent;
    }else if(captureKey == OSET_VALUE){
        if(oset->item == NULL){
            Fatal("Oset item expected to be non-null", TYPE_OSET);
            return ERROR;
        }
        oset->item->content = s;
        oset->item->value = Abs_FromOsetItem(oset->m, oset, oset->item);
    }else if(captureKey == OSET_SEP){
        if((oset->item->type.state & PARENT_TYPE_ARRAY) != 0){
            int idx = -1;
            if(oset->item->key != NULL){
                idx = Int_FromString(oset->item->key);
            }
            if(idx >= 0){
                Span_Set((Span *)oset->item->parent->value, idx, oset->item->value); 
            }else{
                Span_Add((Span *)oset->item->parent->value, oset->item->value); 
            }
        }else if((oset->item->type.state & PARENT_TYPE_TABLE) != 0){
            if(oset->item->key == NULL){
                Fatal("Expected oset key for table sub-item", TYPE_OSET);
                return ERROR;
            }
            Table_Set((Span *)oset->item->parent->value, (Abstract *)oset->item->key, oset->item->value); 
        }
    }
    return SUCCESS;
}

Roebling *OsetParser_Make(MemCtx *m, String *s, Abstract *source){
    MemHandle *mh = (MemHandle *)m;
    Span *parsers_do =  Span_From(m, 7, 
        (Abstract *)Int_Wrapped(m, OSET_MARK_START), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)start),
            (Abstract *)Do_Wrapped(mh, (DoFunc)token),
        (Abstract *)Int_Wrapped(m, OSET_MARK_LENGTH), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)defineLength),
            (Abstract *)Do_Wrapped(mh, (DoFunc)value),
            (Abstract *)Do_Wrapped(mh, (DoFunc)sep));

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
