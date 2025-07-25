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
    r |= Lookup_Add(m, lk, TYPE_BLANK, (void *)OsetDef_Make(m, TYPE_BLANK, String_Make(m, bytes("x")), Blank_FromOset, Blank_ToOset));
    r |= Lookup_Add(m, lk, TYPE_AUTH, (void *)OsetDef_Make(m, TYPE_AUTH, String_Make(m, bytes("auth")), Auth_FromOset, Auth_ToOset));
    r |= Lookup_Add(m, lk, TYPE_ENC_PAIR, (void *)OsetDef_Make(m, TYPE_ENC_PAIR, String_Make(m, bytes("enc")), EncPair_FromOset, EncPair_ToOset));
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

FmtCtx *Oset_Make(MemCtx *m, Lookup *osetDefs){
    FmtCtx *o = FmtCtx_Make(m, NULL);
    o->type.of = TYPE_OSET;
    o->rbl = OsetParser_Make(m, NULL, (Abstract *)o);
    Fmt_Add(m, o, osetDefs);
    return o;
}

String *Oset_To(MemCtx *m, String *key, Abstract *a){
    DebugStack_Push(a, a->type.of);
    if(_oset == NULL){
        Fatal("Oset not intialized", TYPE_OSET);
    }
    Abstract *found = Chain_Get(_oset->resolver->byId, Ifc_Get(a->type.of));
    if(found != NULL){
        FmtDef *def = as(found, TYPE_OSET_DEF);
        DebugStack_Pop();
        return asIfc(def->to(m, def, _oset, key, a), TYPE_STRING);
    }else{
        Debug_Print((void *)a, 0, "Uknown oset type for:" , COLOR_RED, TRUE);
        Fatal("Uknown type for parsing Oset", TYPE_ABSTRACT);
    }
    DebugStack_Pop();
    return NULL;
}

Abstract *Abs_FromOset(MemCtx *m, String *s){
    Oset_Reset(_oset);
    Roebling_Reset(m, _oset->rbl, s);
    _oset->type.state = ZERO;
    _oset->m = m;
    status r = READY;
    while((r & (SUCCESS|ERROR|END)) == 0
            && (_oset->type.state & SUCCESS) == 0){
        r = Roebling_RunCycle(_oset->rbl);
    };

    if((r & ERROR) == 0){
        return Abs_FromOsetItem(m, _oset, _oset->item->def, _oset->item);
    }

    return NULL;
}
