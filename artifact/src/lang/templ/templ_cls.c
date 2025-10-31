#include <external.h>
#include <caneka.h>


static i64 TemplCtx_Print(Buff *bf, Abstract *a, cls type, word flags){
    TemplCtx *ctx = (TemplCtx *)as(a, TYPE_TEMPL_CTX);
    Abstract *args[] = {
        (Abstract *)Type_StateVec(bf->m, ctx->type.of, ctx->type.state),
        (Abstract *)ctx->it.p,
        NULL
    };
    return Fmt(bf, "TemplCtx:<@ @>", args);
}

static i64 TemplItem_Print(Buff *bf, Abstract *a, cls type, word flags){
    Abstract *args[] = {
        (Abstract *)Type_ToStr(bf->m, type),
        (Abstract *)a,
        NULL
    };
    return Fmt(bf, "TemplItem:$<@>", args);
}

static i64 TemplJump_Print(Buff *bf, Abstract *a, cls type, word flags){
    status r = READY;

    TemplJump *jump = (TemplJump *)as(a, TYPE_TEMPL_JUMP);
    Abstract *args[] = {
        (Abstract *)Type_StateVec(bf->m, jump->type.of, jump->type.state),
        (Abstract *)I32_Wrapped(bf->m, jump->idx),
        (Abstract *)I32_Wrapped(bf->m, jump->destIdx),
        (Abstract *)I32_Wrapped(bf->m, jump->skipIdx),
        (Abstract *)I32_Wrapped(bf->m, jump->tempIdx),
        (Abstract *)jump->fch,
        NULL
    };
    return Fmt(bf, "TemplJump:<@ @/dest^D.@^d./skip^D@^d./temp^D@^d. &>", args);
}

static i64 Templ_Print(Buff *bf, Abstract *a, cls type, word flags){
    status r = READY;

    Templ *templ = (Templ *)as(a, TYPE_TEMPL);
    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)Type_StateVec(bf->m, templ->type.of, templ->type.state),
            (Abstract *)&templ->content,
            (Abstract *)&templ->data,
            NULL
        };
        return Fmt(bf, "Templ<@\n  ^E.content^e.:@\n  ^E.data^e.:@>", args);
    }else{
        Abstract *args[] = {
            (Abstract *)Type_StateVec(bf->m, templ->type.of, templ->type.state),
            (Abstract *)I32_Wrapped(bf->m, templ->content.idx),
            NULL
        };
        return Fmt(bf, "Templ<@ content^D.#$^d.>", args);
    }
}

status Templ_ClsInit(MemCh *m){
    status r = READY;
    Lookup *lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_TEMPL, (void *)Templ_Print);
    r |= Lookup_Add(m, lk, TYPE_TEMPL_CTX, (void *)TemplCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_TEMPL_JUMP, (void *)TemplJump_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_VAR, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_TEMPL, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_FOR, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_WITH, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_TEMPL_END, (void *)TemplItem_Print);
    return r;
}
