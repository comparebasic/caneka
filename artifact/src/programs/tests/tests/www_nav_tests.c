#include <external.h>
#include <caneka.h>

static Nav *makeNav(MemCh *m){
    TranspFile *tp = NULL;
    Nav *nav = Nav_Make(m);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Base"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/index.html"));
    Nav_Add(nav, tp->local, (Abstract *)tp);

    Nav *mem = Nav_Make(m);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Mem"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/mem/index.html"));
    Nav_Add(mem, tp->local, (Abstract *)tp);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Iter"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/mem/iter.html"));
    Nav_Add(mem, tp->local, (Abstract *)tp);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "MemCh"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/mem/memch.html"));
    Nav_Add(mem, tp->local, (Abstract *)tp);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "MemBook"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/mem/membook.html"));
    Nav_Add(mem, tp->local, (Abstract *)tp);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "MemPage"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/mem/mempage.html"));
    Nav_Add(mem, tp->local, (Abstract *)tp);
    Nav_Add(nav, Path_Base(m, tp->local), (Abstract *)mem);

    Nav *str = Nav_Make(m);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Str"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/index.html"));
    Nav_Add(str, tp->local, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Str"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/str.html"));
    Nav_Add(str, tp->local, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "StrVec"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/strvec.html"));
    Nav_Add(str, tp->local, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Stream"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/stream.html"));
    Nav_Add(str, tp->local, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Fmt"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/fmt.html"));
    Nav_Add(str, tp->local, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "ToS"));
    tp->local = StrVec_From(m, Str_CstrRef(m, "/base/str/tos.html"));
    Nav_Add(str, tp->local, (Abstract *)tp);

    Nav_Add(nav, Path_Base(m, tp->local), (Abstract *)str);

    return nav;
}

status WwwNav_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    TranspFile *tp = NULL;

    Nav *nav = makeNav(m);

    Abstract *args[] = {
        (Abstract *)nav,
        NULL
    };
    Out("^p.&\n", args);
    
    tp = (TranspFile *)Fetch_Method(m, (Abstract *)nav, Str_CstrRef(m, "index"), NULL);

    Abstract *args1[] = {
        (Abstract *)tp,
        NULL
    };
    r |= Test(tp != NULL && 
        Equals((Abstract *)tp->local, (Abstract *)Str_CstrRef(m, "/base/index.html")),
        "Index method returns expected first item @", args1);

    Iter *it = Fetch_Iter(m, (Abstract *)nav, NULL);
    r |= Test(it != NULL && it->type.of == TYPE_ITER, "Iter returns ITER type", NULL);

    Iter_Next(it);
    Nav *nv = (Nav *)Iter_Get(it);
    Abstract *args2[] = {
        (Abstract *)nv,
        NULL
    };
    r |= Test(nv != NULL && nv->type.of == TYPE_WRAPPED_PTR && 
            ((Single *)nv)->objType.of == TYPE_HTML_NAV, 
        "first item is nav @", args2);

    Str *expected = Str_CstrRef(m, "/base/mem/index.html");
    tp = (TranspFile *)Fetch_Method(m, (Abstract *)nv, Str_CstrRef(m, "index"), NULL);
    Abstract *args3[] = {
        (Abstract *)expected, 
        (Abstract *)tp, 
        NULL
    };
    r |= Test(tp != NULL && 
        Equals((Abstract *)tp->local, (Abstract *)expected),
        "First nav index is expected @ from @", args3);

    MemCh_Free(m);
    return r;
}

status WwwNavTempl_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./docs/html/nav.html"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

    Nav *nav = makeNav(m);
    Stream *sm = Stream_MakeStrVec(m);
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    i64 total = Templ_ToS(templ, sm, (Abstract *)nav, NULL);

    MemCh_Free(m);
    return r;
}
