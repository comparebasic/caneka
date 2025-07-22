#include <external.h>
#include <caneka.h>
#include "include/doc.h" 

static status getHtmlDir(MemCh *m, Str *path){
    return NOOP;
}

static status fileNavFunc(MemCh *m, Str *path, Str *file, Abstract *source){
    DocCtx *ctx = (DocCtx *)as(source, TYPE_DOC_CTX);
    Str *out = (Str *)Table_Get(ctx->args, (Abstract *)Str_CstrRef(m, "out"));
    Str *format = (Str *)Table_Get(ctx->args, (Abstract *)Str_CstrRef(m, "format"));
    TranspFile* tf = TranspFile_Make(m);

    tf->name = Str_Make(m, STR_DEFAULT);

    Str *src = Str_Make(m, STR_DEFAULT);
    Str *dest = Str_Make(m, STR_DEFAULT);

    Str_Add(src, path->bytes, path->length);
    Str_Add(src, (byte *)"/", 1);
    Str_Add(src, file->bytes, file->length);

    Str_Add(dest, out->bytes, out->length);
    Str_Add(dest, path->bytes+ctx->fmtPath->length+1, path->length-ctx->fmtPath->length-1);
    Str_Add(dest, (byte *)"/", 1);

    Str *fname = Str_Clone(m, file);
    Str *ending = Str_CstrRef(m, ".fmt");
    if(Str_EndMatch(fname, ending)){
        Str_Decr(fname, ending->length);
    }else{
        return NOOP;
    }
    Str_Add(tf->name, fname->bytes, fname->length);

    if(Equals((Abstract *)fname, (Abstract *)Str_CstrRef(m, "_"))){
        fname = Str_BuffFromCstr(m, "index");
        tf->type.state |= TRANSP_FILE_INDEX;
    }

    if(Equals((Abstract *)format, (Abstract *)Str_CstrRef(m, "html"))){
        Str_Add(fname, (byte *)".", 1);
        Str_Add(fname, format->bytes, format->length);
    }
    Str_Add(dest, fname->bytes, fname->length);

    Str *localPath = Str_Clone(m, path);
    Str_Incr(localPath, ctx->fmtPath->length+1);
    tf->keys = Str_SplitToSpan(m, localPath, (Abstract *)I8_Wrapped(m, '/'), 
        ZERO);

    Span *keys = Span_Clone(m, tf->keys);
    Span_Add(keys, (Abstract *)tf->name);

    tf->src = File_FnameStrVec(m, src);
    tf->dest = File_FnameStrVec(m, dest);
    Abstract *args[] = {
        (Abstract *)src,
        (Abstract *)tf->src,
        (Abstract *)dest,
        (Abstract *)tf->dest,
        NULL
    };
    Out("^c.& -> &, & -> &^0.\n", args);

    Nested_AddByPath(ctx->nav, keys, (Abstract *)tf);

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
        Dir_CheckCreate(m, out);
        Abstract *args[] = {
            (Abstract *)out,
            NULL
        };
        Out("Makeing destination directory @\n", args);
    }

    Dir_Climb(m, ctx->fmtPath, NULL, fileNavFunc, (Abstract *)ctx); 

    Abstract *args3[] = {
        (Abstract *)ctx->nav,
        NULL
    };
    Out("^y.Nav: &^0.\n", args3);

    DocCtx_GenFiles(ctx);

    DebugStack_Pop();
    return 0;
}
