#include <external.h>
#include "base_module.h"

static Span *identSeps = NULL;

Str *Ident_NameStr(MemCh *m, Ident *ident){
    return Span_HeadStr(ident->name);
}

Str *Ident_ValueStr(MemCh *m, Ident *ident){
    return Span_HeadStr(ident->value);
}

Str *Ident_DomainStr(MemCh *m, Ident *ident){
    return Span_HeadStr(ident->domain);
}

status Ident_Check(void *a){
    return Path_Check(a, identSeps);
}

Ident *Ident_FromVec(MemCh *m, StrVec *v){
    status r = READY;
    Debug_Push(m, v);

    Ident *ident = Ident_Make(m);
    v = Path_Annotate(m, v, identSeps);

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        if(s->type.state & NOOP){
            continue;
        }else if(s->type.state & (MORE|LAST)){
            r = s->type.state;
            continue;
        }

        if(r & MORE){
            Add_OrSpan(m, (void **)&ident->value, s);
        }else if(r & LAST){
            Add_OrSpan(m, (void **)&ident->domain, s);
        }else{
            Add_OrSpan(m, (void **)&ident->name, s);
        }
    }

    Return(m, ident);
}

Ident *Ident_From(MemCh *m, void *name, void *value, void *domain){
    Ident *ident = Ident_Make(m);
    ident->name = (Abstract *)name;
    ident->value = (Abstract *)value;
    ident->domain = (Abstract *)domain;
    return ident;
}

Ident *Ident_Make(MemCh *m){
    Ident *ident = MemCh_AllocOf(m, sizeof(Ident), TYPE_IDENT);
    ident->type.of = TYPE_IDENT;
    return ident;
}

status Ident_Init(MemCh *m){
    if(identSeps == NULL){
        identSeps = Span_Make(m);
        Span_Add(identSeps, B_Wrapped(m, (byte)',', ZERO, NOOP));
        Span_Add(identSeps, B_Wrapped(m, (byte)'=', ZERO, MORE));
        Span_Add(identSeps, B_Wrapped(m, (byte)'@', ZERO, LAST));
        return SUCCESS;
    }
    return NOOP;
}
