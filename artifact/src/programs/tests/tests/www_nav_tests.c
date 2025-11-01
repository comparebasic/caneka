#include <external.h>
#include <caneka.h>

static Nav *makeNav(MemCh *m){
    DebugStack_Push(NULL, ZERO);
    TranspFile *tp = NULL;
    StrVec *path = NULL;
    Nav *nav = Nav_Make(m);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Base"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/index.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;
    printf("I\n");
    fflush(stdout);

    Nav_Add(nav, Path_Base(m, tp->local), (Abstract *)tp);

    printf("II\n");
    fflush(stdout);

    Nav *mem = Nav_Make(m);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Mem"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/mem/index.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    Nav_Add(mem, tp->local, (Abstract *)tp);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Iter"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/mem/iter.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    printf("III\n");
    fflush(stdout);

    Nav_Add(mem, tp->local, (Abstract *)tp);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "MemCh"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/mem/memch.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    Nav_Add(mem, tp->local, (Abstract *)tp);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "MemBook"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/mem/membook.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    printf("IV\n");
    fflush(stdout);

    Nav_Add(mem, tp->local, (Abstract *)tp);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "MemPage"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/mem/mempage.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    Nav_Add(mem, tp->local, (Abstract *)tp);
    Nav_Add(nav, Path_Base(m, tp->local), (Abstract *)mem);

    Nav *str = Nav_Make(m);
    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Str"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/str/index.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    Nav_Add(str, tp->local, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Str"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/str/str.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    Nav_Add(str, tp->local, (Abstract *)tp);

    printf("V\n");
    fflush(stdout);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "StrVec"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/str/strvec.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    Nav_Add(str, tp->local, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Stream"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/str/stream.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    Nav_Add(str, tp->local, (Abstract *)tp);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "Fmt"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/str/fmt.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    Nav_Add(str, tp->local, (Abstract *)tp);

    printf("VI\n");
    fflush(stdout);

    tp = TranspFile_Make(m); 
    tp->name = StrVec_From(m, Str_CstrRef(m, "ToS"));
    path = StrVec_From(m, Str_CstrRef(m, "/base/str/tos.html"));
    IoUtil_Annotate(Nav_MemCh(nav), path);
    tp->local = path;

    printf("VII\n");
    fflush(stdout);

    Nav_Add(str, tp->local, (Abstract *)tp);

    Nav_Add(nav, Path_Base(m, tp->local), (Abstract *)str);

    DebugStack_Pop();
    return nav;
}

status WwwNav_Tests(MemCh *gm){
    DebugStack_Push(NULL, ZERO);
    status r = READY;
    MemCh *m = MemCh_Make();
    TranspFile *tp = NULL;

    Nav *nav = makeNav(m);

    Abstract *args[] = {
        (Abstract *)nav,
        NULL
    };
    Out("^p.&\n", args);
    
    tp = (TranspFile *)Fetch_Prop(m, (Abstract *)nav, Str_CstrRef(m, "index"), NULL);

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
    tp = (TranspFile *)Fetch_Prop(m, (Abstract *)nv, Str_CstrRef(m, "index"), NULL);
    Abstract *args3[] = {
        (Abstract *)expected, 
        (Abstract *)tp, 
        NULL
    };
    r |= Test(tp != NULL && 
        Equals((Abstract *)tp->local, (Abstract *)expected),
        "First nav index is expected @ from @", args3);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status WwwNavTempl_Tests(MemCh *gm){
    DebugStack_Push(NULL, ZERO);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./docs/html/nav.html"));
    StrVec *content = File_ToVec(m, path);
    Cursor *curs = Cursor_Make(m, content);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

    Nav *nav = makeNav(m);
    Buff *bf = Buff_Make(m, ZERO);
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    i64 total = Templ_ToS(templ, bf, (Abstract *)nav, NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
