#include <external.h>
#include <caneka.h>

status WwwRoute_Tests(MemCh *gm){
    DebugStack_Push(NULL, ZERO);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Route *rt = Route_Make(m);

    StrVec *path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "./examples/web-server/pages/public"));
    Route_Collect(rt, path);

    path = StrVec_From(m, Str_FromCstr(m, "/account/", ZERO));
    IoUtil_Annotate(m, path);
    Route *account = Object_ByPath(rt, path, NULL, SPAN_OP_GET);
    Hashed *h = Object_GetByIdx(account, ROUTE_PROPIDX_MIME);
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "text/html", ZERO)), 
        "account index page is mime type text/html", NULL);
    h = Object_GetByIdx(account, ROUTE_PROPIDX_TYPE);
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "html", ZERO)), 
        "account index page is type html", NULL);

    path = StrVec_From(m, Str_FromCstr(m, "/account/profile", ZERO));
    IoUtil_Annotate(m, path);
    Route *profile = Object_ByPath(rt, path, NULL, SPAN_OP_GET);
    h = Object_GetByIdx(profile, ROUTE_PROPIDX_MIME);
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "text/html", ZERO)),
        "profile index page is mime type text/html", NULL);
    h = Object_GetByIdx(profile, ROUTE_PROPIDX_TYPE);
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "templ", ZERO)),
        "profile index page is mime type templ", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
