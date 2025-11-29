#include <external.h>
#include <caneka.h>
#include "module.h"

status BuildCtx_Print(Buff *bf, void *a, cls type, word flags){
    BuildCtx *ctx = (BuildCtx *)as(a, TYPE_BUILDCTX);
    MemCh *m = bf->m;
    if(flags & DEBUG){
        void *args[] = {
            Type_StateVec(m, ctx->type.of, ctx->type.state),
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
            NULL
        };
        return Fmt(bf, "BuildCtx<@\n"
            "  dir:@\nsrc:@\n"
            "  tools: cc:$/$ ar:$\n"
            "  target: @ name:@ source:@ dest:@\n"
            "  input: inc:@ cflags:@ libs:@ sources:@ objects:@ gens:@, srcPrefix:@\n"
            ">", args);
    }else{
        void *args[] = {
            Type_StateVec(m, ctx->type.of, ctx->type.state),
            ctx->dir,
            ctx->src,
            ctx->tools.cc,
            ctx->tools.ccVersion,
            ctx->tools.ar,
            ctx->current.target,
            ctx->current.targetName,
            ctx->current.source,
            ctx->current.dest,
            NULL
        };
        return Fmt(bf, "BuildCtx<@\n"
            "  dir:@\nsrc:@\n"
            "  tools: cc:@/$ ar:@\n"
            "  target: @ name:@ source:@ dest:@\n"
            ">", args);
    }
}

status BuildCtx_ToSInit(MemCh *m){
    status r = READY;
    r |= Lookup_Add(m, ToStreamLookup, TYPE_BUILDCTX, (void *)BuildCtx_Print);
    return r;
}
