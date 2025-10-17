typedef Object Route;

enum route_flags {
    ROUTE_STATIC = 1 << 8,
    ROUTE_DYNAMIC = 1 << 9,
    ROUTE_ASSET = 1 << 10,
    ROUTE_PAGE = 1 << 11,
    ROUTE_SCRIPT = 1 << 12,
};

enum route_prop_idx {
    ROUTE_PROPIDX_PATH = 0,
    ROUTE_PROPIDX_FILE = 1,
    ROUTE_PROPIDX_FUNC = 2,
    ROUTE_PROPIDX_MIME = 3,
    ROUTE_PROPIDX_TYPE = 4,
};

extern struct span *RouteFuncTable;
extern struct span *RouteMimeTable;

typedef status (*RouteFunc)(MemCh *m, 
    StrVec *in, StrVec *out, Object *data, Abstract *source);

Route *Route_Make(MemCh *m);
status Route_Collect(Route *rt, StrVec *path);
status Route_ClsInit(MemCh *m);
status Route_SetTargetFile(Route *rt, Str *ext, Str *absPath);
status Route_Handle(MemCh *m, Route *rt, StrVec *dest, Object *data, Abstract *source);
