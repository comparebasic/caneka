#include <external.h>
#include "buildeka_module.h"

status BuildCtx_SetupModules(BuildCtx *ctx){
    status r = READY;
    MemCh *m = ctx->m;
    Debug_Push(m, ctx);
    Iter it;
    Iter_Init(&it, ctx->deps);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it); 
        if(h != NULL){
            void *ar[] = {
                h->key,
                h->value,
                NULL
            };
            Out("^p.Gathering Deps^0\n", ar);
        }
    }

    Return(m, r);
}

status BuildCtx_SetFlag(BuildCtx *ctx, StrVec *flag){
    if(Span_Has(ctx->input.cflags, flag) == -1){
        Span_Add(ctx->input.cflags, flag);
        return SUCCESS;
    }
    return NOOP;
}

status BuildCtx_Build(BuildCtx *ctx){

    status r = READY;
    MemCh *m = ctx->m;
    Debug_Push(m, ctx);

    BuildCtx_Config(ctx);
    exit(1);
    /*

    Time_Now(&ctx->start);

    BuildCtx_GenAllIncSpan(ctx);

    Iter it;
    Iter_Init(&it, ctx->input.sources);
    while((Iter_Next(&it) & END) == 0){
        StrVec *v = StrVec_From(m, Iter_Get(&it));
        IoUtil_Annotate(m, v);
        StrVec *key = StrVec_Make(m);
        StrVec_AddVecAfter(key, v, ctx->input.srcPrefix->p->nvalues+1);
        BuildCtx_ParseDependencies(ctx, key, v);
    }

    if(ctx->type.state & DEBUG){
        void *args[] = {ctx, NULL};
        Out("^p.Ctx after dependencies &^0\n", args);
    }

    ctx->input.countModules->val.i = 0;
    StrVec_Add(ctx->current.source, IoUtil_PathSep(m));
    Iter_Init(&it, Table_Ordered(m, ctx->input.dependencies));
    while((Iter_Prev(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            ctx->input.countModules->val.i = it.p->nvalues - it.idx;

            IoUtil_TrimDir(m, h->key);
            if(BuildCtx_BuildModule(ctx,
                (StrVec *)h->key, (DirSel *)h->value) & ERROR){
                r |= ERROR;
                break;
            }
        }
    }


    Return(m, r);
    */
    return ZERO;
}

BuildCtx *BuildCtx_Make(MemCh *m){
    Debug_Push(m, NULL);

    BuildCtx *ctx = MemCh_AllocOf(m, sizeof(BuildCtx), TYPE_BUILDCTX);
    ctx->type.of = TYPE_BUILDCTX;
    ctx->m = MemCh_Make();
    ctx->cli.cli = CliStatus_Make(m, BuildCli_RenderStatus, ctx);

    ctx->dir = StrVec_Make(m);
    ctx->src = StrVec_Make(m);
    ctx->deps = Table_Make(m);

    ctx->current.target = StrVec_Make(m);
    ctx->current.targetName = StrVec_Make(m);
    ctx->current.version = StrVec_Make(m);
    ctx->current.source = StrVec_Make(m);
    ctx->current.dest = StrVec_Make(m);

    ctx->input.inc = Span_Make(m);
    ctx->input.cflags = Span_Make(m);
    ctx->input.libs = Span_Make(m);
    ctx->input.staticLibs = Span_Make(m);
    ctx->input.sources = Span_Make(m);
    ctx->input.objects = Span_Make(m);
    ctx->input.gens = Span_Make(m);

    ctx->tools.cc = S(m, _gen_CC);
    ctx->tools.ccVersion = Str_FromI64(m, (i64)_gen_CC_VERSION);
    ctx->tools.ar = S(m, _gen_AR);

    Return(m, ctx);
}
