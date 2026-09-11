#include <external.h>
#include "buildeka_module.h"

status BuildCtx_SetFlags(BuildCtx *ctx){
    MemCh *m = ctx->m;
    ctx->current.flags = Span_Make(m);
    void *args[4];

    args[0] = ctx->dest;
    args[1] = NULL;
    Span_Add(ctx->current.flags, S(m, "-I"));
    Span_Add(ctx->current.flags, Fmt_ToStrVec(m, "$/include/", args));

    args[0] = ctx->src;
    args[1] = NULL;
    Span_Add(ctx->current.flags, S(m, "-I"));
    Span_Add(ctx->current.flags, Fmt_ToStrVec(m, "$/api/include/", args));

    Iter it;
    Iter_Init(&it, ctx->depsOrdered);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            BuildModule *md = (BuildModule *)h->value;
            args[0] = md->src;
            args[1] = md->name;
            args[2] = NULL;
            Span_Add(ctx->current.flags, S(m, "-I"));
            Span_Add(ctx->current.flags, Fmt_ToStrVec(m, "$/include/", args));
        }
    }

    Iter_Init(&it, ctx->options);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        Str *opt = NULL;
        if(a->type.of == TYPE_STR){
            opt = (Str *)a;
        }else if(a->type.of == TYPE_IDENT){
            Ident *ident = (Ident *)a;
            opt = (Str *)Ifc(m, ident->name, TYPE_STR);
        }
        if(opt != NULL){
            opt = Str_ToUpper(m, opt);
            args[0] = opt;
            args[1] = NULL;
            Span_Add(ctx->current.flags, S(m, "-D"));
            Span_Add(ctx->current.flags, Fmt_ToStrVec(m, "CNKOPT_$", args));
        }
    }

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
    return ZERO;
}

status BuildCtx_Build(BuildCtx *ctx){

    status r = READY;
    MemCh *m = ctx->m;
    Debug_Push(m, ctx);

    BuildCtx_Config(ctx);
    BuildCtx_SetLogging(ctx);

    Iter_Init(&ctx->current.moduleIt, ctx->depsOrdered);
    while((Iter_Next(&ctx->current.moduleIt) & END) == 0){
        Hashed *h = Iter_Get(&ctx->current.moduleIt);
        if(h != NULL){
            BuildCtx_SetFlags(ctx);

            void *ar[] = {
                ctx->current.flags,
                NULL
            };
            Out("^y.Flags @^0\n", ar);

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
                    BuildObject *obj = BuildObject_Current(m, ctx);
                    void *ar[] = {
                        obj,
                        NULL
                    };
                    Out("^c.Building @^0\n", ar);
                }

            }
        }
    }
    return ZERO;
}

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
