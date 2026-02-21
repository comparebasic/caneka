#include <external.h>
#include "buildeka_module.h"

Span *BuildCtx_ToModDeclare(MemCh *m, Str *s){
    Span *declare = Span_Make(m);
    byte *b = s->bytes;
    byte *end = s->bytes+s->length-1;
    Str *shelf = Str_Make(m, STR_DEFAULT);
    while(b <= end){
        byte c = *b;
        if(c == '@'){
            Span_Set(declare, BUILD_MOD_DECLARE_LABEL, Str_Clone(m, shelf));
            Str_Wipe(shelf);
        }else if(c == '='){
            Span_Set(declare, BUILD_MOD_DECLARE_TAG, Str_Clone(m, shelf));
            Str_Wipe(shelf);
        }else{
            Str_Add(shelf, b, 1);
        }

        b++;
    }

    if(shelf->length > 0){
        if(Span_Get(declare, BUILD_MOD_DECLARE_TAG) == NULL){
            Span_Set(declare, BUILD_MOD_DECLARE_TAG, S(m, "dep"));
        }
        while(declare->max_idx < BUILD_MOD_DECLARE_VALUE){
            Span_Add(declare, shelf);
        }
    }

    return declare;
}

Table *BuildCtx_GenOptionsTable(BuildCtx *ctx, Span *p){
    Table *tbl = Table_Make(ctx->m);
    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        Span *p = BuildCtx_ToModDeclare(ctx->m, s);
        Str *label = Span_Get(p, BUILD_MOD_DECLARE_LABEL);
        Str *value = Span_Get(p, BUILD_MOD_DECLARE_VALUE);
        Table_Set(tbl, label, value);
    }
    return tbl;
}

status BuildCtx_ParseDependencies(BuildCtx *ctx, StrVec *key, StrVec *path){
    DebugStack_Push(NULL, ZERO);
    void *args[5];
    MemCh *m = ctx->m;

    if(ctx->type.state & DEBUG){
        void *ar[] = {key, NULL}; 
        Out("^b.Parsing Key @^0.\n", ar);
    }

    i32 anchor = StrVec_AddVecAfter(   
        ctx->current.source,
        path,
        ctx->input.srcPrefix->p->nvalues);

    StrVec_Add(ctx->current.source, IoUtil_PathSep(m));
    StrVec_Add(ctx->current.source, S(m, "mod"));
    StrVec_Add(ctx->current.source, IoUtil_PathSep(m));

    Str *pathS = StrVec_Str(m, path);
    DirSel *sel = NULL;
    if(Dir_Exists(m, pathS) & SUCCESS){
        sel = DirSel_Make(m,
            S(m, ".c"), NULL, DIR_SELECTOR_MTIME_ALL|DIR_SELECTOR_NODIRS);
        StrVec *base = StrVec_Copy(m, path);
        StrVec_Add(base, IoUtil_PathSep(m));
        StrVec_Add(base, S(m, "option"));

        Span *filter = Span_Make(m);
        Span_Add(filter, StrVec_Str(m, base));
        sel->type.state |= DIR_SELECTOR_INVERT;
        sel->source = filter;

        Dir_GatherFilterDir(m, pathS, sel);
        if(ctx->input.options != NULL && ctx->input.options->nvalues > 0){

            filter = Span_Make(m);
            Iter it;
            Iter_Init(&it, ctx->input.options);
            while((Iter_Next(&it) & END) == 0){
                Hashed *h = Iter_Get(&it);
                if(h != NULL){
                    StrVec *opt = StrVec_From(m, h->key);
                    StrVec *v = StrVec_Copy(m, base);
                    IoUtil_Annotate(m, opt);
                    StrVec_Add(v, IoUtil_PathSep(m));
                    StrVec_AddVec(v, opt);
                    Span_Add(filter, StrVec_Str(m, v));
                }
            }

            sel->type.state &= ~DIR_SELECTOR_INVERT;
            sel->source = filter;

            void *args[] = {sel, pathS, NULL};
            Out("^y.Gathering @ @^0.",  args);
            exit(1);

            Dir_GatherFilterDir(m, pathS, sel);
        }

        StrVec *name = StrVec_Make(m);
        StrVec_AddVecAfter(name, path, ctx->input.srcPrefix->p->nvalues+1);

        if(Table_Get(ctx->input.dependencies, name) != NULL){
            DebugStack_Pop();
            return NOOP;
        }

        ctx->input.totalModules->val.i++;
        Table_Set(ctx->input.dependencies, name, sel);

        if(Time_Greater(&sel->time, &ctx->modified)){
            ctx->modified = sel->time;
            void *ar[] = {Time_ToStr(m, &ctx->modified), NULL};
        }

        ctx->input.totalSources->val.i += sel->dest->nvalues;
    }else{
        args[0] = path;
        args[1] = ctx;
        args[2] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Dependency not found @ for @", args);
        DebugStack_Pop();
        return ERROR;
    }

    StrVec_Add(ctx->current.source, K(m, "dependencies.txt"));

    Buff *bf = Buff_Make(m, ZERO|BUFF_SLURP);
    bf->type.state |= NOOP;
    File_Open(bf, StrVec_Str(m, ctx->current.source), O_RDONLY);
    if(bf->type.state & ERROR){
        DebugStack_Pop();
        return NOOP; 
    }
    Buff_Read(bf);
    File_Close(bf);

    StrVec_PopTo(ctx->current.source, anchor);

    Str *shelf = Str_Make(m, STR_DEFAULT);
    Cursor *curs = Cursor_Make(m, bf->v);
    Str *dep = NULL;
    while((Cursor_NextByte(curs) & END) == 0){
        dep = NULL;
        if(*curs->ptr == '\n'){
            Span *declare = BuildCtx_ToModDeclare(ctx->m, shelf);
            Str_Wipe(shelf);

            Str *tag = Span_Get(declare, BUILD_MOD_DECLARE_TAG);
            Str *label = Span_Get(declare, BUILD_MOD_DECLARE_LABEL);
            Str *value = Span_Get(declare, BUILD_MOD_DECLARE_VALUE);
            Table_SetInTable(sel->meta, tag, label, value);

            if(Equals(tag, K(m, "option"))){
                Hashed *h = Table_GetHashed(ctx->input.options, label);
                if(h != NULL && Equals(h->value, value)){
                    Table_SetInTable(sel->meta, S(m, "api"), label, value);
                    dep = value;
                }else{
                    continue;
                }
            } else if(Equals(tag, K(m, "dep"))){
                dep = Span_Get(declare, BUILD_MOD_DECLARE_VALUE);
            }
            Table_SetInTable(sel->meta, tag, label, value);

            if(dep != NULL){
                Table_SetInTable(sel->meta, S(m, "dep"), label, value);

                StrVec *depV = StrVec_From(m, dep);
                IoUtil_Annotate(m, depV);

                path = StrVec_Copy(m, ctx->input.srcPrefix);
                StrVec_Add(path, IoUtil_PathSep(m));
                StrVec_Add(ctx->current.source, S(m, "mod"));
                StrVec_Add(ctx->current.source, IoUtil_PathSep(m));
                StrVec_AddVec(path, depV);
                IoUtil_Annotate(ctx->m, path);

                BuildCtx_ParseDependencies(ctx, depV, path);
            }
        }else{
            Str_Add(shelf, curs->ptr, 1);
        }
    }

    if(ctx->type.state & DEBUG){
        args[0] = sel;
        args[1] = NULL;
        Out("^p.Sel @^0\n", args);
    }

    StrVec_PopTo(ctx->current.source, anchor);
    DebugStack_Pop();
    return ZERO;
}
