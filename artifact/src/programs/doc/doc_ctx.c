#include <external.h>
#include <caneka.h>
#include "include/doc.h" 

Table *DocCtx_ArgResolve(DocCtx *ctx, int argc, char *argv[]){
    MemCh *m = ctx->m;
    Table *argResolve = Table_Make(m);
    Single *True = I32_Wrapped(m, TRUE);

    Single *debugArg = Ptr_Wrapped(m, NULL, TYPE_UNKNOWN);
    debugArg->type.state |= ARG_OPTIONAL;
    Table_Set(argResolve, (Abstract *)Str_CstrRef(m, "debug"), (Abstract *)debugArg);
    
    Table_Set(argResolve,  (Abstract *)Str_CstrRef(m, "in"), (Abstract *)True);

    Table *args = Table_Make(m);
    if(CharPtr_ToTbl(m, argResolve, argc, argv, args) & (ERROR|NOOP)){
        CharPtr_ToHelp(m, Str_CstrRef(m, "doc"), argResolve, argc, argv);
        exit(1);
    }
    return args;
}

status DocCtx_SetFmtPath(DocCtx *ctx, Str *path){
    ctx->fmtPath = File_GetAbsPath(ctx->m, path);
    return ctx->fmtPath->type.state;
}

DocCtx *DocCtx_Make(MemCh *m){
    DocCtx *ctx = (DocCtx *)MemCh_Alloc(m, sizeof(DocCtx));
    ctx->type.of = TYPE_DOC_CTX;
    ctx->m = m;
    ctx->nav = Nested_Make(m);
    ctx->pages = OrdTable_Make(m);
    ctx->modules = OrdTable_Make(m);
    ctx->fmtPath = Str_Make(m, STR_DEFAULT);
    return ctx;
}
