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
        BuildModule *md = (BuildModule *)Iter_Get(&it);
        void *ar[] = {
            md->name,
            NULL
        };
        Fmt(bf, "#include <$_module.h>\n", ar);
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
        BuildModule *md = (BuildModule *)Iter_Get(&it);
        if(md->type.state & BUILDMODULE_HEADER_CHANGE){
            r |= BUILDMODULE_UPSTREAM_CHANGE;
            continue;
        }
        if(r != ZERO){
            md->type.state &= ~BUILDMODULE_SATISFIED;
        }
        md->type.state |= r;
    }
    return ZERO;
}

status BuildCtx_ShowStatus(BuildCtx *ctx){
    MemCh *m = ctx->m;
    BuildModule *md = Span_Get(ctx->depsOrdered, ctx->depsOrdered->max_idx);
    void *args[6];
    Iter it;

    args[0] = md->local;
    args[1] = NULL;
    Out("\n^pD.^d.Building the following modules/sources for ^D.$^d.:\n", args);

    Iter_Init(&it, ctx->depsOrdered);
    Out("  ^p.Order: ", NULL);
    while((Iter_Next(&it) & END) == 0){
        BuildModule *md = (BuildModule *)Iter_Get(&it);
        args[0] = md->name;
        args[1] = NULL;
        if(it.type.state & LAST){
            Out("$.", args);
        }else{
            Out("$, ", args);
        }
    }
    Out("\n", NULL);

    Span *build = Span_Make(m);
    Span *update = Span_Make(m);
    Span *skip = Span_Make(m);

    Iter_Init(&it, ctx->depsOrdered);
    while((Iter_Next(&it) & END) == 0){
        BuildModule *md = (BuildModule *)Iter_Get(&it);
        if(md->type.state & BUILDOBJ_SATISFIED){
            Span_Add(skip, md);
        }else if(md->metrics.built > 0){
            Span_Add(update, md);
        }else{
            Span_Add(build, md);
        }
    }

    if(skip->nvalues > 0){
        args[0] = I32_Wrapped(m, skip->nvalues);
        args[1] = NULL;
        Out("\n  ^g.Skipping ^D.$^d. modules: not rebuilding, newest target already exists:\n", args);

        Iter_Init(&it, skip);
        while((Iter_Next(&it) & END) == 0){
            BuildModule *md = (BuildModule *)Iter_Get(&it);
            args[0] = md->name;
            args[1] = md->targetName;
            args[2] = NULL;
            Out("    |\n    + ^D.$^d. -> (^D.$^d.).\n", args);
        }
    }

    if(update->nvalues > 0){
        args[0] = I32_Wrapped(m, update->nvalues);
        args[1] = NULL;
        Out("\n  ^c.Updating ^D.$^d. modules:\n", args);

        Str *reason = S(m, "");
        if(md->type.state & BUILDMODULE_UPSTREAM_CHANGE){
            reason = S(m, "upstream changed");
        }else if(md->type.state & BUILDMODULE_HEADER_CHANGE){
            reason = S(m, "headers changed");
        }else if(md->type.state & BUILDMODULE_SOURCE_CHANGE){
            reason = S(m, "sources changed");
        }

        Iter_Init(&it, update);
        while((Iter_Next(&it) & END) == 0){
            BuildModule *md = (BuildModule *)Iter_Get(&it);
            args[0] = md->name;
            args[1] = I32_Wrapped(m, md->metrics.sources - md->metrics.built);
            args[2] = I32_Wrapped(m, md->metrics.sources);
            args[3] = md->targetName;
            args[4] = reason;
            args[5] = NULL;
            Out("    |\n    + ^D.$^d. -> rebuilding ^D.$^d. of $ objects (^D.$^d.): $.\n", args);
        }
    }

    if(build->nvalues > 0){
        args[0] = I32_Wrapped(m, build->nvalues);
        args[1] = NULL;
        Out("\n  ^y.Building ^D.$^d. modules:\n", args);

        Str *reason = S(m, "");
        if(md->type.state & BUILDMODULE_UPSTREAM_CHANGE){
            reason = S(m, "upstream changed");
        }else if(md->type.state & BUILDMODULE_HEADER_CHANGE){
            reason = S(m, "headers changed");
        }else if(md->type.state & BUILDMODULE_SOURCE_CHANGE){
            reason = S(m, "sources changed");
        }

        Iter_Init(&it, build);
        while((Iter_Next(&it) & END) == 0){
            BuildModule *md = (BuildModule *)Iter_Get(&it);
            args[0] = md->name;
            args[1] = I32_Wrapped(m, md->metrics.sources);
            args[2] = md->targetName;
            args[3] = reason;
            args[4] = NULL;
            Out("    |\n    + ^D.$^d. -> building ^D.$^d. objects (^D.$^d.): $.\n", args);
        }
    }

    Out("\n^0.\n", args);
    return ZERO;
}

status BuildCtx_Build(BuildCtx *ctx){
    status r = READY;
    MemCh *m = ctx->m;
    Debug_Push(m, ctx);

    BuildCtx_MakeInclude(ctx);

    Iter_Init(&ctx->current.moduleIt, ctx->depsOrdered);
    while((Iter_Next(&ctx->current.moduleIt) & END) == 0){
        BuildModule *md = (BuildModule *)Iter_Get(&ctx->current.moduleIt);
        if((md->type.state & BUILDMODULE_SATISFIED) == 0){
            BuildModule_BuildCurrent(ctx);
        }
        /*
        md->m->level--;
        MemCh_FreeTemp(md->m);
        */
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
