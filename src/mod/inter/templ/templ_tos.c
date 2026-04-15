#include <external.h>
#include <caneka.h>

static status TemplCtx_Print(Buff *bf, void *a, cls type, word flags){
    TemplCtx *ctx = (TemplCtx *)Ifc(bf->m, a, TYPE_TEMPL_CTX);
    void *args[] = {
        Type_StateVec(bf->m, ctx->type.of, ctx->type.state),
        ctx->it.p,
        NULL
    };
    return Fmt(bf, "TemplCtx:<@ @>", args);
}

static status TemplItem_Print(Buff *bf, void *a, cls type, word flags){
    void *args[] = {
        Type_ToStr(bf->m, type),
        a,
        NULL
    };
    return Fmt(bf, "TemplItem:$<@>", args);
}

static status TemplCrit_Print(Buff *bf, void *a, cls type, word flags){
    TemplCrit *crit = (TemplCrit *)a;
    status r = READY;
    void *args[6];
    r |= Fmt(bf, "TCrit<", args);
    if(crit->type.state){
        args[0] = Type_StateVec(bf->m, TYPE_ITER_UPPER, crit->type.state);
        args[1] = NULL;
        r |= Fmt(bf, "@ ", args);
    }
    if(crit->dflag.positive || crit->dflag.negative){
        args[0] = Type_StateVec(bf->m, TYPE_ITER_UPPER, crit->dflag.positive);
        args[1] = Type_StateVec(bf->m, TYPE_ITER_UPPER, crit->dflag.negative);
        args[2] = NULL;
        r |= Fmt(bf, "+@/-@ ", args);
    }
    args[0] = I32_Wrapped(bf->m, crit->contentIdx);
    args[1] = I32_Wrapped(bf->m, crit->dataIdx);
    args[2] = NULL;
    r |= Fmt(bf, "^D.$^d./data$>", args);
    return r;
}

static status TemplFunc_Print(Buff *bf, void *a, cls type, word flags){
    TemplFunc *tfunc = (TemplFunc *)a;
    void *args[5];
    args[0] = Type_StateVec(bf->m, TYPE_ITER_UPPER, tfunc->type.state);
    args[1] = Type_StateVec(bf->m, TYPE_ITER_UPPER, tfunc->dflag.positive);
    args[2] = Type_StateVec(bf->m, TYPE_ITER_UPPER, tfunc->dflag.negative);
    args[3] = Util_Wrapped(bf->m, (util)tfunc->func);
    args[4] = NULL;
    return Fmt(bf, "TFunc<@ +@/-@ func(@)>", args);
}

static status Jumps_Print(Buff *bf, void *a, cls type, word flags){
    status r = READY;
    Jumps *js = (Jumps*)a;
    Str **labels = Lookup_Get(ToSFlagLookup, TYPE_ITER_UPPER);
    void *args[3];

    StrVec *v = StrVec_Make(bf->m);
    args[0] = Type_StateVec(bf->m, TYPE_ITER_UPPER, js->type.state),
    args[1] = I32_Wrapped(bf->m, js->idx),
    args[2] = NULL;

    r |= Fmt(bf, "Jumps<@\\@$ ", args);
    boolean first = TRUE;
    word one = 1;
    for(i32 i = 0; i < 8; i++){
        if(js->crit[i] != NULL){
            if(first){
                first = FALSE;
            }else{
                r |= Buff_AddBytes(bf, (byte *)",", 1);
            }
            args[0] = labels[i+9];
            args[2] = NULL;
            Abstract *a = (Abstract *)js->crit[i];
            if(a->type.of == TYPE_TEMPL_JUMP_CRIT){
                args[1] = a;
                r |= Fmt(bf, "$/@", args);
            }else if(a->type.of == TYPE_ITER){
                Iter *it = (Iter *)a;
                args[1] = it->p;
                r |= Fmt(bf, "$/@", args);
            }

        }
    }
    r |= Buff_AddBytes(bf, (byte *)">", 1);

    return r;
}

static status Templ_Print(Buff *bf, void *a, cls type, word flags){
    status r = READY;

    Templ *templ = (Templ *)Ifc(bf->m, a, TYPE_TEMPL);
    if(flags & DEBUG){
        void *args[] = {
            Type_StateVec(bf->m, templ->type.of, templ->type.state),
            &templ->content,
            &templ->data,
            templ->jumps,
            templ->funcs,
            NULL
        };
        return Fmt(bf, "Templ<@\n"
            "  ^E.content^e.:@\n"
            "  ^E.data^e.:@\n"
            "  ^E.jumps^e.:&\n"
            "  ^E.funcs^e.:@>"
            , args);
    }else{
        void *args[] = {
            Type_StateVec(bf->m, templ->type.of, templ->type.state),
            I32_Wrapped(bf->m, templ->content.idx),
            NULL
        };
        return Fmt(bf, "Templ<@ content^D.#$^d.>", args);
    }
}

status Templ_Init(MemCh *m){
    status r = READY;
    r |= Templ_FuncInit(m);
    Lookup *lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_TEMPL, (void *)Templ_Print);
    r |= Lookup_Add(m, lk, TYPE_TEMPL_CTX, (void *)TemplCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_TEMPL_JUMPS, (void *)Jumps_Print);
    r |= Lookup_Add(m, lk, TYPE_TEMPL_JUMP_CRIT, (void *)TemplCrit_Print);
    r |= Lookup_Add(m, lk, TYPE_TEMPL_JUMP_FUNC, (void *)TemplFunc_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_VAR, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_TEMPL, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_FOR, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_WITH, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_TEMPL_TEMPL_END, (void *)TemplItem_Print);
    return r;
}
