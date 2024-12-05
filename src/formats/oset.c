#include <external.h>
#include <caneka.h>

/* example
oset/:s/5=title;kv/:=s/3=hey;a:b64/12=XafjkacC;;;

oset
    string = title
    key value = 
        string = hey -> address = (base64) XafjkacC
*/

static FmtCtx *_oset = NULL;

FmtDef *OsetDef_Make(MemCtx *m, cls typeOf, String *name, FmtTrans from, FmtTrans to){
    FmtDef *def = (FmtDef *)MemCtx_Alloc(m, TYPE_OSET_DEF);
    def->type.of = TYPE_OSET_DEF;
    def->id = typeOf;
    def->name = name;
    def->from = from;
    def->to = to;
    return def;
}

status Oset_Reset(FmtCtx *o){
    o->item = NULL;
    o->type.state &= ~(SUCCESS|ERROR);
    return SUCCESS;
}

static status populateOset(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING, (void *)OsetDef_Make(m, TYPE_STRING, String_Make(m, bytes("s")), String_FromOset, String_ToOset));
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)OsetDef_Make(m, TYPE_TABLE, String_Make(m, bytes("tbl")), Table_FromOset, Iter_ToOset));
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)OsetDef_Make(m, TYPE_SPAN, String_Make(m, bytes("span")), Span_FromOset, Iter_ToOset));
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I64, (void *)OsetDef_Make(m, TYPE_WRAPPED_I64, String_Make(m, bytes("i64")), I64_FromOset, I64_ToOset));
    r |= Lookup_Add(m, lk, TYPE_FILE, (void *)OsetDef_Make(m, TYPE_FILE, String_Make(m, bytes("file")), FilePath_FromOset, FilePath_ToOset));
    return r;
}

status Oset_Init(MemCtx *m){
    if(_oset == NULL){
        Lookup *osetDefs = Lookup_Make(m, _TYPE_START, populateOset, NULL);
        _oset = Oset_Make(m, osetDefs);
        return SUCCESS;
    }
    return NOOP;
}

status Oset_Add(MemCtx *m, FmtCtx *o, Lookup *osetDefs){
    Iter *it = Iter_Make(m, osetDefs->values);
    Lookup *byId = Lookup_Make(m, osetDefs->offset, NULL, NULL);
    Span *byName = Span_Make(m, TYPE_TABLE);
    while((Iter_Next(it) & END) == 0){
        FmtDef *def = (FmtDef *)Iter_Get(it);
        if(def != NULL){
            Table_Set(byName, (Abstract *)def->name, (Abstract *)def);
            Lookup_Add(m, byId, def->id, (Abstract *)def);
        }
    }
    if(o->byId == NULL){
        o->byId = Chain_Make(m, byId);
    }else{
        Chain_Extend(m, o->byId, byId);
    }
    if(o->byName == NULL){
        o->byName = TableChain_Make(m, byName);
    }else{
        TableChain_Extend(m, o->byName,  byName);
    }
    return SUCCESS;
}

FmtCtx *Oset_Make(MemCtx *m, Lookup *osetDefs){
    FmtCtx *o = FmtCtx_Make(m);
    o->type.of = TYPE_OSET;
    o->rbl = OsetParser_Make(m, NULL, (Abstract *)o);
    Oset_Add(m, o, osetDefs);
    return o;
}

String *Oset_To(MemCtx *m, String *key, Abstract *a){
    if(_oset == NULL){
        Fatal("Oset not intialized", TYPE_OSET);
    }
    Abstract *found = Chain_Get(_oset->byId, Ifc_Get(a->type.of));
    if(found != NULL){
        FmtDef *def = as(found, TYPE_OSET_DEF);
        return asIfc(def->to(m, def, _oset, key, a), TYPE_STRING);
    }else{
        Debug_Print((void *)a, 0, "Uknown oset type for:" , COLOR_RED, TRUE);
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

    if((r & ERROR) == 0){
        return Abs_FromOsetItem(m, _oset, _oset->item->def, _oset->item);
    }

    return NULL;
}
