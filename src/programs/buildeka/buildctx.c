#include <external.h>
#include "buildeka_module.h"

status BuildCtx_SetFlags(BuildCtx *ctx){
    MemCh *m = ctx->m;
    return ZERO;
}

status BuildCtx_MakeInclude(BuildCtx *ctx){
    MemCh *m = ctx->m;
    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
    StrVec *path = Clone(m, ctx->dest);
    IoUtil_AddVec(m, path, Sv(m, "include/caneka.h"));
    void *ar[] = {
        path,
        NULL
    };
    Out("Making dir for $^0\n", ar);
    Dir_CheckCreateFor(m, path);
    File_Open(bf, Ifc(m, path, TYPE_STR), O_WRONLY|O_CREAT);
    Buff_Add(bf, S(m, "/* Caneka.h - main header file for building Caneka */\n\n#ifndef CANEKA_H\n#define CANEKA_H\n\n"));

    Iter it;
    Iter_Init(&it, ctx->depsOrdered);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            BuildModule *md = (BuildModule *)h->value;
            void *ar[] = {
                md->name,
                NULL
            };
            Fmt(bf, "#include <$_module.h>\n", ar);
        }
    }

    Buff_Add(bf, S(m, "\n#endif\n"));
    File_Close(bf);

    return ZERO;
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

status BuildCtx_Setup(BuildCtx *ctx){
    status r = READY;
    MemCh *m = ctx->m;
    Debug_Push(m, ctx);

    BuildCtx_Config(ctx);
    BuildCtx_SetLogging(ctx);
    BuildCtx_SetFlags(ctx);

    return ZERO;
}

status BuildCtx_SetStatus(BuildCtx *ctx){
    MemCh *m = ctx->m;
    status r = READY;
    Iter it;
    Iter_Init(&it, ctx->depsOrdered);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            BuildModule *md = (BuildModule *)h->value;
            if(md->type.state & BUILDMODULE_HEADER_CHANGE){
                r |= BUILDMODULE_UPSTREAM_CHANGE;
                continue;
            }
            if(r != ZERO){
                md->type.state &= ~BUILDMODULE_SATISFIED;
            }
            md->type.state |= r;
        }
    }
    return ZERO;
}

status BuildCtx_ShowStatus(BuildCtx *ctx){
    MemCh *m = ctx->m;
    Iter it;
    Iter_Init(&it, ctx->depsOrdered);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            BuildModule *md = (BuildModule *)h->value;
            if(md->type.state & BUILDOBJ_SATISFIED){
                void *ar[] = {
                    md->name,
                    NULL
                };
                Out("^pD.$^d. -> not rebuilding^0\n", ar);
            }else{
                Str *reason = S(m, "");
                if(md->type.state & BUILDMODULE_UPSTREAM_CHANGE){
                    reason = S(m, 
                        "Upstream Changed - rebuilding because of something upstream.");
                }else if(md->type.state & BUILDMODULE_INC){
                    reason = S(m, "Inc - using inc.c to build as includes.");
                }else if(md->type.state & BUILDMODULE_HEADER_CHANGE){
                    reason = S(m, "Header Changed - rebuilding.");
                }else if(md->type.state & BUILDMODULE_SOURCE_CHANGE){
                    reason = S(m, "Sources Changed - rebuilding select sources.");
                }
                void *ar[] = {
                    md->name,
                    I32_Wrapped(m, md->metrics.sources - md->metrics.built),
                    I32_Wrapped(m, md->metrics.sources),
                    reason,
                    NULL
                };
                Out("^pD.$^d. -> building $ of $ sources: $^0\n", ar);
            }
        }
    }
    return ZERO;
}

status BuildCtx_Build(BuildCtx *ctx){
    status r = READY;
    MemCh *m = ctx->m;
    Debug_Push(m, ctx);

    BuildCtx_MakeInclude(ctx);

    Iter_Init(&ctx->current.moduleIt, ctx->depsOrdered);
    while((Iter_Next(&ctx->current.moduleIt) & END) == 0){
        Hashed *h = Iter_Get(&ctx->current.moduleIt);
        if(h != NULL){
            BuildModule *md = (BuildModule *)h->value;
            if((md->type.state & BUILDMODULE_SATISFIED) == 0){
                BuildModule_BuildCurrent(ctx);
            }
            /*
            md->m->level--;
            MemCh_FreeTemp(md->m);
            */
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
