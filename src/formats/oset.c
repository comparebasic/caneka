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

OsetDef *OsetDef_Make(MemCtx *m, cls typeOf, String *name, Maker from, ToOset to){
    OsetDef *def = (OsetDef *)MemCtx_Alloc(m, TYPE_OSET_DEF);
    def->type.of = TYPE_OSET_DEF;
    def->typeOf = typeOf;
    def->name = name;
    def->from = from;
    def->to = to;
    return def;
}

status OsetDef_Reset(Oset *o){
    o->remaining = 0;
    o->currentType = ZERO;
}

status Oset_Init(MemCtx *m){
    if(_oset == NULL){
        Span *osetDefs = Span_Make(m, TYPE_SPAN);
        Span_Add(osetDefs, (Abstract *)OsetDef_Make(m, TYPE_STRING, String_Make(m, bytes("s"), String_FromOset, String_ToOset)));
        Span_Add(osetDefs, (Abstract *)OsetDef_Make(m, TYPE_TABLE, String_Make(m, bytes("s"), String_FromOset, String_ToOset)));
        Span_Add(osetDefs, (Abstract *)OsetDef_Make(m, TYPE_SPAN, String_Make(m, bytes("s"), String_FromOset, String_ToOset)));
        _oset = Oset_Make(osetDefs);
        return SUCCESS;
    }
    return NOOP;
}

status Oset_Add(MemCtx *m, Oset *o, Lookup *osetDefs){
    Iter *it = Iter_Make(m, osetDefs->values);
    Abstract *first = Span_Get(osetDefs->values, 0);
    Lookup *byType = Lookup_Make(m, first->type.of);
    Table *byName = Span_Make(m, TYPE_TABLE);
    while((Iter_Next(it) & END) == 0){
        OsetDef *def = (OsetDef *)Iter_Get(it);
        Table_Set(byName, def->name, (Abstract *)def);
        Lookup_Add(m, byType, def->typeOf, (Abstract *)def);
    }
    Chain_Extend(m, o->byType, byType);
    TableChain_Extend(m, o->byName, byName);
    return SUCCESS;
}

Oset *Oset_Make(MemCtx *m, Lookup *osetDefs){
    Oset *o = (Oset *)MemCtx_Alloc(m, sizeof(Oset));
    o->type.of = TYPE_OSET;
    o->rbl = OsetParser_Make(m, NULL, o);
    Oset_Add(MemCtx *m, o, osetDefs);
    return o;
}

String *Oset_To(MemCtx *m, cls type, Abstract *a){
    if(_oset == NULL){
        Fatal("Oset not intialized", TYPE_OSET);
    }
    if(type == 0){
        type = a->type.of;
    }
    Abstract *found = Chain_Get(_oset->byType, type);
    OsetDef *def = as(found, TYPE_OSET_DEF);
    if(def != NULL){
        return def->to(m, a);
    }else{
        Fatal("Uknown type for parsing Oset", TYPE_ABSTRACT);
    }
}

Abstract *Abs_FromOset(MemCtx *m, String *s){
    Roebling_Reset(m, _oset->rbl, s);
    _oset->type.state = ZERO;
    while(!HasFlag(xml->rbl->type.state, BREAK)
            && !HasFlag(_oset->type.state, SUCCESS)){
        Roebling_RunCycle(xml->rbl);
    };

    if((_oset->rbl->type.state & SUCCESS) != 0){
        printf("yay parset oset\n");
    }

    return NULL;
}
