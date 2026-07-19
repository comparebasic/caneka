#include <external.h>
#include <caneka.h>

Str *Inst_GetSilt(MemCh *m, Inst *inst){
    if((inst->type.of & TYPE_INSTANCE) == 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Silt can only be generated for an instance", NULL);
    }

    Span *p = Span_Make(m);
    Str *s = Str_Make(m, sizeof(cls) * inst->nvalues);
    s->type.state |= STRING_BINARY;

    Iter it;
    Iter_Init(&it, inst);
    Str_Add(s, (byte *)&inst->type.of, sizeof(word));
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = (Abstract *)Iter_Get(&it);
        Str_Add(s, (byte *)&a->type.of, (i16)sizeof(word));
        printf("Adding prop %d/%d\n", (i32)s->length, (i32)s->alloc);
        fflush(stdout);
    }

    return s;
}

void Silt_AddProp(Span *p, cls typeOf){
    Span_Add(p, I16_Wrapped(p->m, typeOf));
}

Str *Silt_FromSpan(MemCh *m, Span *p){
    Str *s = Str_Make(m, sizeof(cls) * p->nvalues);
    s->type.state |= STRING_BINARY;

    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Single *sg = (Single *)Iter_Get(&it);
        Str_Add(s, (byte *)&sg->val.w, sizeof(word));
    }
    return s;
}
