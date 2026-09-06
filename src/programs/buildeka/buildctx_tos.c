#include <external.h>
#include "buildeka_module.h"

static Str **moduleLabels = NULL;


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
            ctx->dir,
            ctx->src,
            ctx->tools.cc,
            ctx->tools.ccVersion,
            ctx->tools.ar,
            ctx->current.target,
            ctx->current.targetName,
            ctx->current.source,
            ctx->current.dest,
            ctx->input.inc,
            ctx->input.cflags,
            ctx->input.libs,
            ctx->input.sources,
            ctx->input.objects,
            ctx->input.gens,
            ctx->input.srcPrefix,
            ctx->options,
            Table_Ordered(bf->m, ctx->deps),
            NULL
        };
        return Fmt(bf, "BuildCtx<@ @ $\n"
            "  dir:@\nsrc:@\n"
            "  tools: cc:$/$ ar:$\n"
            "  target: @\n      name:@\n      source:@\n      dest:@\n"
            "  input: inc:@ cflags:@ libs:@ sources:@ objects:@ gens:@, srcPrefix:@\n"
            "    options: @\n"
            "    deps: @\n"
            ">", args);
    }else{
        void *args[] = {
            Type_StateVec(m, ctx->type.of, ctx->type.state),
            Time_ToStr(m, &ctx->modified),
            ctx->ident,
            ctx->dir,
            ctx->src,
            ctx->tools.cc,
            ctx->tools.ccVersion,
            ctx->tools.ar,
            ctx->current.target,
            ctx->current.targetName,
            ctx->current.source,
            ctx->current.dest,
            ctx->input.buildDir,
            ctx->options,
            Table_Ordered(bf->m, ctx->deps),
            NULL
        };
        return Fmt(bf, "BuildCtx<@ @ $\n"
            "  dir:@\nsrc:@\n"
            "  tools: cc:@/$ ar:@\n"
            "  target: @ name:@ source:@ dest:@\n"
            "  buildDir: @\n"
            "    options: @\n"
            "    deps: @\n"
            ">", args);
    }
    return ZERO;
}

status BuildCtx_ToSInit(MemCh *m){
    status r = READY;
    if(moduleLabels == NULL){
        moduleLabels = (Str **)Arr_Make(m, 17);
        moduleLabels[9] = Str_CstrRef(m, "INC");
        Lookup_Add(m, ToSFlagLookup, TYPE_BUILD_MODULE, (void *)moduleLabels);
        r |= SUCCESS;
    }
    r |= Lookup_Add(m, ToStreamLookup, TYPE_BUILDCTX, (void *)BuildCtx_Print);
    r |= Lookup_Add(m, ToStreamLookup, TYPE_BUILD_MODULE, (void *)BuildModule_Print);
    return r;
}
