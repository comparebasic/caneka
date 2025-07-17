#include <external.h>
#include <caneka.h>
#include "include/doc.h" 

static status getHtmlDir(MemCh *m, Str *path){
    return NOOP;
}

static status dirFunc(MemCh *m, Str *path, Abstract *source){
    Str *newPath = Str_CloneAlloc(m, path, STR_DEFAULT);
    Str *replace = Str_CstrRef(m, "fmt");
    Span *backlog = Span_Make(m);
    Match *mt = Match_Make(m, PatChar_FromStr(m, replace), backlog);
    mt->type.state |= MATCH_SEARCH;
    Str *new = Str_CstrRef(m, "html");
    i32 pos = 0;
    Match_StrReplace(m, newPath, new, mt, &pos);

    Abstract *args[] = {
        (Abstract *)mt,
        (Abstract *)path,
        (Abstract *)newPath,
        NULL
    };
    Out("dirFunc: & @ making @\n", args);
    return Dir_CheckCreate(m, newPath);
}

static status fileFunc(MemCh *m, Str *path, Str *file, Abstract *source){
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

    Dir_Climb(m, ctx->fmtPath, dirFunc, fileFunc, NULL); 

    DebugStack_Pop();
    return 0;
}
