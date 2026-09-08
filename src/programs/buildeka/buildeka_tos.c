#include <external.h>
#include "buildeka_module.h"

static Str **moduleLabels = NULL;

status BuildObject_Print(Buff *bf, void *a, cls type, word flags){
    BuildObject *obj = (BuildObject *)a;
    void *ar[] = {
        obj->src,
        obj->dest,
        NULL
    };
    return Fmt(bf, "Object<@ -> @>", ar);
}

status BuildModule_Print(Buff *bf, void *a, cls type, word flags){
    BuildModule *md = (BuildModule *)a;
    void *ar[] = {
        Type_StateVec(bf->m, md->type.of, md->type.state),
        md->name,
        md->targetName,
        Time_ToRStr(bf->m, &md->latest),
        md->src,
        md->target,
        md->sel != NULL && md->sel->dest != NULL ?
            I32_Wrapped(bf->m, md->sel->dest->nvalues) : I32_Wrapped(bf->m, 0),
        md->config,
        NULL
    };
    return Fmt(bf, "BuildModule<@ @/@ (@) <- @ -> @ = Sel(@files) config(@)>", ar);
}

status BuildCtx_Print(Buff *bf, void *a, cls type, word flags){
    BuildCtx *ctx = (BuildCtx *)Ifc(bf->m, a, TYPE_BUILDCTX);
    MemCh *m = bf->m;
    if(flags & DEBUG){
        void *args[] = {
            Type_StateVec(m, ctx->type.of, ctx->type.state),
            Time_ToStr(m, &ctx->modified),
            ctx->ident,
            ctx->dest,
            ctx->src,
            ctx->tools.cc,
            ctx->tools.ccVersion,
            ctx->tools.ar,
            ctx->options,
            Table_Ordered(bf->m, ctx->deps),
            NULL
        };
        return Fmt(bf, "BuildCtx<@ @ $\n"
            "  dest:@\nsrc:@\n"
            "  tools: cc:$/$ ar:$\n"
            "  deps: @\n"
            ">", args);
    }else{
        void *args[] = {
            Type_StateVec(m, ctx->type.of, ctx->type.state),
            Time_ToStr(m, &ctx->modified),
            ctx->ident,
            ctx->dest,
            ctx->src,
            ctx->tools.cc,
            ctx->tools.ccVersion,
            ctx->tools.ar,
            ctx->options,
            Table_Ordered(bf->m, ctx->deps),
            NULL
        };
        return Fmt(bf, "BuildCtx<@ @ $\n"
            "  dest:@\nsrc:@\n"
            "  tools: cc:@/$ ar:@\n"
            "  options: @\n"
            "  deps: @\n"
            ">", args);
    }
    return ZERO;
}

status BuildCtx_ToSInit(MemCh *m){
    status r = READY;
    if(moduleLabels == NULL){
        moduleLabels = (Str **)Arr_Make(m, 17);
        moduleLabels[9] = Str_CstrRef(m, "INC");
        moduleLabels[10] = Str_CstrRef(m, "SATISFIED");
        Lookup_Add(m, ToSFlagLookup, TYPE_BUILD_MODULE, (void *)moduleLabels);
        r |= SUCCESS;
    }
    r |= Lookup_Add(m, ToStreamLookup, TYPE_BUILDCTX, (void *)BuildCtx_Print);
    r |= Lookup_Add(m, ToStreamLookup, TYPE_BUILD_MODULE, (void *)BuildModule_Print);
    r |= Lookup_Add(m, ToStreamLookup, TYPE_BUILD_OBJECT, (void *)BuildObject_Print);
    return r;
}
