#include <external.h>
#include <caneka.h>

static i64 TemplItem_Print(Stream *sm, Abstract *a, cls type, word flags){
    Abstract *args[] = {
        (Abstract *)Type_ToStr(sm->m, type),
        (Abstract *)a,
        NULL
    };
    return Fmt(sm, "TemplItem:$<@>", args);
}

static i64 TemplJump_Print(Stream *sm, Abstract *a, cls type, word flags){
    status r = READY;

    TemplJump *jump = (TemplJump *)as(a, TYPE_TEMPL_JUMP);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)jump, ToS_FlagLabels),
        (Abstract *)I32_Wrapped(sm->m, jump->idx),
        (Abstract *)I32_Wrapped(sm->m, jump->destIdx),
        (Abstract *)I32_Wrapped(sm->m, jump->skipIdx),
        (Abstract *)jump->fch,
        NULL
    };
    return Fmt(sm, "TemplJump:<$ @/dest^D.@^d./skip^D@^d. &>", args);
}

static i64 Templ_Print(Stream *sm, Abstract *a, cls type, word flags){
    status r = READY;

    Templ *templ = (Templ *)as(a, TYPE_TEMPL);
    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)templ, ToS_FlagLabels),
            (Abstract *)&templ->content,
            (Abstract *)&templ->data,
            NULL
        };
        return Fmt(sm, "Templ<$\n  ^E.content^e.:@\n  ^E.data^e.:@>", args);
    }else{
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)templ, ToS_FlagLabels),
            (Abstract *)I32_Wrapped(sm->m, templ->content.idx),
            NULL
        };
        return Fmt(sm, "Templ<$ content^D.#$^d.>", args);
    }
}

status Templ_ClsInit(MemCh *m){
    status r = READY;
    Lookup *lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_TEMPL, (void *)Templ_Print);
    r |= Lookup_Add(m, lk, TYPE_TEMPL_JUMP, (void *)TemplJump_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_VAR, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_TEMPL, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_FOR, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_WITH, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_TEMPL_END, (void *)TemplItem_Print);
    return r;
}
