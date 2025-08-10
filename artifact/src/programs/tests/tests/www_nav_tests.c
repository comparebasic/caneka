#include <external.h>
#include <caneka.h>
#include <www.h>

status WwwNav_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    TranspFile *tp = NULL;

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./docs/html/nav.html"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

    Nav *nav = Nav_Make(m);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Base"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/index.html"));
    Nav_Add(nav, (Abstract *)tp);

    Nav *mem = Nav_Make(m);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Mem"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/mem.html"));
    Nav_Add(mem, (Abstract *)tp);
    Nav_Add(nav, (Abstract *)mem);

    Nav *str = Nav_Make(m);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Str"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/index.html"));
    Nav_Add(str, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Str"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/str.html"));
    Nav_Add(str, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "StrVec"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/strvec.html"));
    Nav_Add(str, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Stream"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/stream.html"));
    Nav_Add(str, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Fmt"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/fmt.html"));
    Nav_Add(str, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "ToS"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/tos.html"));
    Nav_Add(str, (Abstract *)tp);

    Nav_Add(nav, (Abstract *)str);
    Abstract *args[] = {
        (Abstract *)nav,
        NULL
    };
    Out("^p.&\n", args);
    /*

    OrdTable_Set(data, (Abstract *)Str_CstrRef(m, "menu-items"), (Abstract *)nav);

    Stream *sm = Stream_MakeStrVec(m);
    
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    i64 total = Templ_ToS(templ, sm, data, NULL);

    Str *expected = Str_CstrRef(m, logicTestContent);
    Abstract *args[] = {
        (Abstract *)expected,
        (Abstract *)sm->dest.curs->v,
        NULL
    };
    r |= TestShow(Equals((Abstract *)expected, (Abstract *)sm->dest.curs->v), 
        "Temple key value test has expected content", 
        "Temple key value test mismatch, expected $\nhave\n$", 
        args);

    */

    MemCh_Free(m);
    return r;
}

