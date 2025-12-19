#include <external.h>
#include "buildeka_module.h"

status BuildCtx_ParseDependencies(BuildCtx *ctx, StrVec *key, StrVec *path){
    DebugStack_Push(NULL, ZERO);
    void *args[5];
    MemCh *m = ctx->m;

    if(ctx->type.state & DEBUG){
        void *ar[] = {key, NULL}; 
        Out("^b.Parsing Key @^0.\n", ar);
    }

    Table *options = Table_FromSpan(m, ctx->input.options);

    i32 anchor = StrVec_AddVecAfter(   
        ctx->current.source,
        path,
        ctx->input.srcPrefix->p->nvalues);

    StrVec_Add(ctx->current.source, IoUtil_PathSep(m));

    Str *pathS = StrVec_Str(m, path);
    DirSelector *sel = NULL;
    if(Dir_Exists(m, pathS) & SUCCESS){
        sel = DirSelector_Make(m,
            S(m, ".c"), NULL, DIR_SELECTOR_MTIME_ALL|DIR_SELECTOR_NODIRS);
        if(ctx->input.options != NULL && ctx->input.options->nvalues > 0){
            StrVec *base = StrVec_Clone(m, path);
            StrVec_Add(base, IoUtil_PathSep(m));
            StrVec_Add(base, S(m, "option"));

            Span *filter = Span_Make(m);
            Span_Add(filter, StrVec_Str(m, base));
            sel->type.state |= DIR_SELECTOR_INVERT;
            sel->source = filter;

            Dir_GatherFilterDir(m, pathS, sel);

            filter = Span_Make(m);
            Iter it;
            Iter_Init(&it, ctx->input.options);
            while((Iter_Next(&it) & END) == 0){
                StrVec *opt = StrVec_From(m, Iter_Get(&it));
                StrVec *v = StrVec_Copy(m, base);
                IoUtil_Annotate(m, opt);
                StrVec_Add(v, IoUtil_PathSep(m));
                StrVec_AddVec(v, opt);
                Span_Add(filter, StrVec_Str(m, v));
            }

            sel->type.state &= ~DIR_SELECTOR_INVERT;
            sel->source = filter;
            Dir_GatherFilterDir(m, pathS, sel);
        }else{
            Dir_GatherSel(m, pathS, sel);
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
    Str *tag = NULL;
    Str *name = NULL;
    while((Cursor_NextByte(curs) & END) == 0){
        if(*curs->ptr == '\n'){
            if(tag != NULL){
                if(Equals(tag, K(m, "option"))){
                    if(name != NULL){
                        if(Table_Get(options, name) != NULL){
                            Table_SetInTable(sel->meta, tag, name, shelf);
                            name = shelf;
                            Table_SetInTable(sel->meta, S(m, "api"), name, shelf);
                            goto dep;
                        }else{
                            goto next;
                        }
                    }else if(Table_Get(options, shelf) != NULL){
                        Table_SetInTable(sel->meta, tag, shelf, shelf);
                        name = shelf;
                        goto dep;
                    }else{
                        goto next;
                    }
                }else{
                    Table_SetInTable(sel->meta, tag, shelf, shelf);
                    goto next;
                }
            }else{
                name = shelf;
            }
dep:
            Table_SetInTable(sel->meta, S(m, "dep"), name, shelf);

            StrVec *dep = StrVec_From(m, name);
            IoUtil_Annotate(m, dep);

            path = StrVec_Copy(m, ctx->input.srcPrefix);
            StrVec_Add(path, IoUtil_PathSep(m));
            StrVec_AddVec(path, dep);
            IoUtil_Annotate(ctx->m, path);

            BuildCtx_ParseDependencies(ctx, dep, path);

            tag = NULL;
            name = NULL;
            shelf = Str_Make(m, STR_DEFAULT);
        }else if(*curs->ptr == '@'){
            name = shelf;
            shelf = Str_Make(m, STR_DEFAULT);
        }else if(*curs->ptr == '='){
            tag = shelf;
            shelf = Str_Make(m, STR_DEFAULT);
        }else{
            Str_Add(shelf, curs->ptr, 1);
        }
        continue;
next:        
        tag = NULL;
        name = NULL;
        shelf = Str_Make(m, STR_DEFAULT);
    }

    StrVec_PopTo(ctx->current.source, anchor);
    DebugStack_Pop();
    return ZERO;
}
