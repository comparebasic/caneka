
#include <external.h>
#include <caneka.h>

status TemplRoute_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    StrVec *path = StrVec_From(m, Str_CstrRef(m,
        "examples/web-server/pages/inc/header.templ"));
    StrVec *abs = IoUtil_AbsVec(m, path);

    Route *rt = (Object *)Route_Make(m);
    StrVec *home = StrVec_From(m, Str_CstrRef(m, "/"));
    Object_SetPropByIdx(rt, ROUTE_PROPIDX_PATH, (Abstract *)home);

    StrVec *navPath = StrVec_From(m, Str_CstrRef(m,
        "examples/web-server/pages/public"));
    StrVec *navAbs = IoUtil_AbsVec(m, navPath);
    Route_Collect(rt, navAbs);

    Object *data = Object_Make(m, ZERO);
    Object_Set(data, (Abstract *)Str_CstrRef(m, "menu-items"), (Abstract *)rt);
    Str *now = MicroTime_ToStr(m, MicroTime_Now());
    Object_Set(data, (Abstract *)Str_CstrRef(m, "now"), (Abstract *)now);
    StrVec *title = StrVec_From(m, Str_CstrRef(m, "The Title of the Master Page"));
    Object_Set(data, (Abstract *)Str_CstrRef(m, "title"), (Abstract *)title);

    args[0] = (Abstract *)abs;
    args[1] = (Abstract *)data;
    args[2] = NULL;
    Out("^y.Transpiling @ with data:@^0.\n", args);

    StrVec *v = File_ToVec(m, StrVec_Str(m, abs));

    Cursor *curs = Cursor_Make(m, v);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    Templ_Prepare(templ);

    StrVec *out = StrVec_Make(m);
    Stream *sm = Stream_MakeToVec(m, out);
    Templ_ToS(templ, sm, (Abstract *)data, NULL);

    args[0] = (Abstract *)out;
    args[1] = NULL;
    Out("^y.Output of template &^0.\n", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
