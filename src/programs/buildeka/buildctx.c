#include <external.h>
#include "buildeka_module.h"

status BuildCtx_SetFlag(BuildCtx *ctx, StrVec *flag){
    /*
    if(Span_Has(ctx->input.cflags, flag) == -1){
        Span_Add(ctx->input.cflags, flag);
        return SUCCESS;
    }
    */
    return NOOP;
}

status BuildCtx_SetLogging(BuildCtx *ctx){
    MemCh *m = ctx->m;
    Iter it;
    Iter_Init(&it, Table_Ordered(m, ctx->deps));
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        BuildModule *md = (BuildModule *)h->value;
        ctx->metrics.modules++;
        ctx->metrics.sources += md->metrics.sources;
    }

    if(Ansi_HasColor(OutStream)){
        BuildCli_SetupStatus(ctx);
        ctx->cli = CliStatus_Make(m, BuildCli_RenderStatus, ctx);
        ctx->log = BuildCli_Log;
    }else{
        ctx->log = BuildLogger_Log;
    }
}

status BuildCtx_Build(BuildCtx *ctx){

    status r = READY;
    MemCh *m = ctx->m;
    Debug_Push(m, ctx);

    BuildCtx_Config(ctx);
    BuildCtx_SetLogging(ctx);

    Iter it;
    Iter_Init(&ctx->current.moduleIt, ctx->depsOrdered);
    while((Iter_Next(&ctx->current.moduleIt) & END) == 0){
        Hashed *h = Iter_Get(&it);
        BuildModule *md = (BuildModule *)h->value;
        if((md->type.state & BUILDMODULE_SATISFIED) == 0){
            void *ar[] = {
                md->name,
                Type_StateVec(m, md->type.of, md->type.state),
                md->targetName, 
                Time_ToRStr(m, &md->latest),
                I32_Wrapped(m, md->metrics.sources),
                md->src,
                md->target,
                NULL
            };
            Out("^p.Building @/@ -> ^D.$^d. latest(@) files:@ -> \n  $ -> $^0\n", ar);

            if(md->sel == NULL || md->sel->dest == NULL){
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Empty DirSel no file count to build", NULL);
                return ERROR;
            }

            Iter_Init(&ctx->current.sourcesIt, md->sel->dest);
            while((Iter_Next(&ctx->current.sourcesIt) & END) == 0){
                BuildObject *obj = BuildObject_Make(m,
                    ctx, ctx->current.moduleIt.idx, ctx->current.sourcesIt.idx);
                
                void *ar[] = {
                    obj,
                    NULL
                };
                Out("^y.Building @^0\n", ar);
            }

        }
    }
}

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

BuildCtx *BuildCtx_Make(MemCh *m){
    Debug_Push(m, NULL);

    BuildCtx *ctx = MemCh_AllocOf(m, sizeof(BuildCtx), TYPE_BUILDCTX);
    ctx->type.of = TYPE_BUILDCTX;
    ctx->m = MemCh_Make();

    ctx->dest = StrVec_Make(m);
    ctx->src = StrVec_Make(m);
    ctx->deps = Table_Make(m);

    ctx->tools.cc = S(m, _gen_CC);
    ctx->tools.ccVersion = Str_FromI64(m, (i64)_gen_CC_VERSION);
    ctx->tools.ar = S(m, _gen_AR);

    Return(m, ctx);
}
