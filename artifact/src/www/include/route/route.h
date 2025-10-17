typedef Object Route;

enum route_prop_idx {
    ROUTE_PROPIDX_PATH = 0,
    ROUTE_PROPIDX_HTML = 1,
    ROUTE_PROPIDX_ACTION = 2,
    ROUTE_PROPIDX_FMT = 3,
    ROUTE_PROPIDX_TEAMPL = 4,
};

Route *Route_Make(MemCh *m);
status Route_Collect(Route *rt, StrVec *path);
status Route_ClsInit(MemCh *m);
status Route_SetTargetFile(Route *rt, Str *ext, Str *absPath);
