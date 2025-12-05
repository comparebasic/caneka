#include <external.h>
#include "cnkbuild_module.h"

status BuildCtx_ParseDependencies(BuildCtx *ctx, StrVec *key, StrVec *path){
    DebugStack_Push(NULL, ZERO);
    void *args[5];
    MemCh *m = ctx->m;

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
        Dir_GatherSel(m, pathS, sel);
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
        if(!Equals(key, path)){
            Span *p = NULL;
            Str *meta = S(m, "choice");
            if((p = Table_Get(sel->meta, meta)) == NULL){
                p = Span_Make(m);
                Table_Set(sel->meta, meta, p);
            }
            key->type.state |= BUILD_CHOICE;
            Span_Add(p, name);
        }
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
    Str *label = NULL;
    Str *name = NULL;
    while((Cursor_NextByte(curs) & END) == 0){
        if(*curs->ptr == '\n'){
            if(label != NULL && label->length > 0 ||
                    name != NULL && name->length > 0){
                if(Equals(label, K(m, "choice"))){
                    if(name != NULL && name->length > 0){
                        key = StrVec_From(m, Str_Clone(m, name));
                        Table_SetInTable(sel->meta, S(m, "provides"), key, key);
                    }
                }else{
                    Str *meta = label;
                    if(Equals(label, K(m, "exec"))){
                        meta->type.state |= BUILD_EXEC; 
                    }else if(Equals(label, K(m, "static"))){
                        meta->type.state |= BUILD_STATIC; 
                    }else if(Equals(label, K(m, "link"))){
                        meta->type.state |= BUILD_LINK; 
                    }else if(Equals(label, K(m, "skip"))){
                        meta->type.state |= BUILD_SKIP; 
                        ctx->input.totalSources->val.i--;
                    }else if(Equals(label, K(m, "include"))){
                        meta->type.state |= BUILD_INCLUDE; 
                    }

                    Table_SetInTable(sel->meta, meta, shelf, shelf);

                    label = NULL;
                    name = NULL;
                    shelf = Str_Make(m, STR_DEFAULT);
                    continue;
                }
            }else{
                key = NULL;
            }

            Table_SetInTable(sel->meta, S(m, "dep"), shelf, shelf);

            path = StrVec_Copy(m, ctx->input.srcPrefix);
            StrVec_Add(path, IoUtil_PathSep(m));
            StrVec_Add(path, shelf);
            IoUtil_Annotate(ctx->m, path);
            if(key == NULL){
                key = path;
            }

            BuildCtx_ParseDependencies(ctx, key, path);
            label = NULL;
            name = NULL;
            shelf = Str_Make(m, STR_DEFAULT);
        }else if(*curs->ptr == '@'){
            name = shelf;
            shelf = Str_Make(m, STR_DEFAULT);
        }else if(*curs->ptr == '='){
            label = shelf;
            shelf = Str_Make(m, STR_DEFAULT);
        }else{
            Str_Add(shelf, curs->ptr, 1);
        }
    }

    StrVec_PopTo(ctx->current.source, anchor);
    DebugStack_Pop();
    return ZERO;
}
