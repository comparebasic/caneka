#include <external.h>
#include <caneka.h>

/* example
oset/:s/5=title;kv/:=s/3=hey;a:b64/12=XafjkacC;;;

oset
    string = title
    key value = 
        string = hey -> address = (base64) XafjkacC
*/

static Oset *_oset = NULL;

OsetDef *OsetDef_Make(MemCtx *m, cls typeOf, String *name, OsetTrans from, OsetTrans to){
    OsetDef *def = (OsetDef *)MemCtx_Alloc(m, TYPE_OSET_DEF);
    def->type.of = TYPE_OSET_DEF;
    def->typeOf = typeOf;
    def->name = name;
    def->fromOset = from;
    def->toOset = to;
    return def;
}

status Oset_Reset(Oset *o){
    o->content = NULL;
    o->key = NULL;
    o->odef = NULL;
    o->remaining = 0;
    o->type.state &= ~(SUCCESS|ERROR);
    return SUCCESS;
}

static status populateOset(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING, (void *)OsetDef_Make(m, TYPE_STRING, String_Make(m, bytes("s")), String_FromOset, String_ToOset));
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)OsetDef_Make(m, TYPE_TABLE, String_Make(m, bytes("tbl")), Table_FromOset, Iter_ToOset));
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)OsetDef_Make(m, TYPE_SPAN, String_Make(m, bytes("span")), Span_FromOset, Iter_ToOset));
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I64, (void *)OsetDef_Make(m, TYPE_WRAPPED_I64, String_Make(m, bytes("i64")), I64_FromOset, I64_ToOset));
    return r;
}

status Oset_Init(MemCtx *m){
    if(_oset == NULL){
        Lookup *osetDefs = Lookup_Make(m, _TYPE_START, populateOset, NULL);
        Debug_Print((void *)osetDefs, 0, "OsetDefs: ", COLOR_CYAN, TRUE);
        printf("\n");
        _oset = Oset_Make(m, osetDefs);
        return SUCCESS;
    }
    return NOOP;
}

status Oset_Add(MemCtx *m, Oset *o, Lookup *osetDefs){
    Iter *it = Iter_Make(m, osetDefs->values);
    Lookup *byType = Lookup_Make(m, osetDefs->offset, NULL, NULL);
    Span *byName = Span_Make(m, TYPE_TABLE);
    while((Iter_Next(it) & END) == 0){
        OsetDef *def = (OsetDef *)Iter_Get(it);
        if(def != NULL){
            Table_Set(byName, (Abstract *)def->name, (Abstract *)def);
            Lookup_Add(m, byType, def->typeOf, (Abstract *)def);
        }
    }
    if(o->byType == NULL){
        o->byType = Chain_Make(m, byType);
    }else{
        Chain_Extend(m, o->byType, byType);
    }
    if(o->byName == NULL){
        o->byName = TableChain_Make(m, byName);
    }else{
        TableChain_Extend(m, o->byName,  byName);
    }
    return SUCCESS;
}

Oset *Oset_Make(MemCtx *m, Lookup *osetDefs){
    Oset *o = (Oset *)MemCtx_Alloc(m, sizeof(Oset));
    o->type.of = TYPE_OSET;
    o->m = m;
    o->rbl = OsetParser_Make(m, NULL, (Abstract *)o);
    Oset_Add(m, o, osetDefs);
    return o;
}

String *Oset_To(MemCtx *m, String *key, Abstract *a){
    if(_oset == NULL){
        Fatal("Oset not intialized", TYPE_OSET);
    }
    Abstract *found = Chain_Get(_oset->byType, a->type.of);
    OsetDef *def = as(found, TYPE_OSET_DEF);
    if(def != NULL){
        return as(def->toOset(m, def, _oset, key, a), TYPE_STRING);
    }else{
        Fatal("Uknown type for parsing Oset", TYPE_ABSTRACT);
    }
    return NULL;
}

Abstract *Abs_FromOset(MemCtx *m, String *s){
    Oset_Reset(_oset);
    Roebling_Reset(m, _oset->rbl, s);
    _oset->type.state = ZERO;
    _oset->m = m;
    status r = READY;
    while((r & (SUCCESS|ERROR|BREAK)) == 0
            && !HasFlag(_oset->type.state, SUCCESS)){
        r = Roebling_RunCycle(_oset->rbl);
    };

    if((_oset->rbl->type.state & SUCCESS) != 0){
        OsetDef *odef = _oset->odef;
        if(odef == NULL){
            Fatal("OsetDef not found", TYPE_OSET);
        }
        return odef->fromOset(m, odef, _oset, _oset->key, (Abstract *)_oset->content);
    }

    return NULL;
}
