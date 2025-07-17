#include <external.h>
#include <caneka.h>
#include "include/doc.h" 

static status getHtmlDir(MemCh *m, Str *path){
    return NOOP;
}

static status dirNavFunc(MemCh *m, Str *path, Abstract *source){
    DocCtx *ctx = (DocCtx *)as(source, TYPE_DOC_CTX);
    Str *out = (Str *)Table_Get(ctx->args, (Abstract *)Str_CstrRef(m, "out"));
    Str *newPath = Str_Make(m, STR_DEFAULT);
    Str_Add(newPath, out->bytes, out->length);
    Str *localPath = Str_Clone(m, path);
    Str_Incr(localPath, ctx->fmtPath->length+1);
    Str_Add(newPath, localPath->bytes, localPath->length);
    /*
    Str_Add(newPath, out->bytes, out->length);
    Str *localPath = Str_Clone(m, path);
    Str_Incr(localPath, ctx->fmtPath->length);
    Str_Add(newPath, localPath->bytes, localPath->length);
    Str_AddCstr(newPath, ".html");
    */

    Abstract *args[] = {
        (Abstract *)path,
        (Abstract *)newPath,
        NULL
    };
    Out("dirFunc: & -> &\n", args);
    return SUCCESS;
    /*
    return Dir_CheckCreate(m, newPath);
    */
}

static status fileNavFunc(MemCh *m, Str *path, Str *file, Abstract *source){
    Abstract *args[] = {
        (Abstract *)path,
        (Abstract *)file,
        NULL
    };
    Out("fileFunc: @ -> @\n", args);
    return SUCCESS;
}

i32 main(int argc, char **argv){
    MemBook *cp = MemBook_Make(NULL);
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);

    DebugStack_Push(NULL, 0);

    DocCtx *ctx = DocCtx_Make(m);
    ctx->args = DocCtx_ArgResolve(ctx, argc, argv);
    DocCtx_SetFmtPath(ctx, Str_CstrRef(m, "docs/fmt")); 

    Abstract *_args[] = {
        (Abstract *)ctx->args,
        (Abstract *)ctx->fmtPath,
        NULL
    };

    Out("^y.Args @ and module path: @^0.\n", _args);
    Str *out = (Str *)Table_Get(ctx->args, (Abstract *)Str_CstrRef(m, "out"));
    if((Dir_Exists(m, out) & SUCCESS) == 0){
        Abstract *args[] = {
            (Abstract *)out,
            NULL
        };
        Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
            "Out directory does not exist: @", args);
        exit(1);
    }

    Dir_Climb(m, ctx->fmtPath, dirNavFunc, fileNavFunc, (Abstract *)ctx); 

    DebugStack_Pop();
    return 0;
}
