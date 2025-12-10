typedef Inst Route;

enum route_flags {
    ROUTE_STATIC = 1 << 8,
    ROUTE_DYNAMIC = 1 << 9,
    ROUTE_FMT = 1 << 10,
    ROUTE_INDEX = 1 << 13,
    ROUTE_BINSEG = 1 << 11,
    ROUTE_ASSET = 1 << 12,
    ROUTE_ACTION = 1 << 14,
    ROUTE_FORBIDDEN = 1 << 15,
};

enum route_prop_idx {
    ROUTE_PROPIDX_PATH = 0,
    ROUTE_PROPIDX_DATA = 1,
    ROUTE_PROPIDX_CHILDREN = 2,
    /* NodeObj end */
    ROUTE_ROUTE_GENS = 3,
};

extern struct span *RouteFuncTable;
extern struct span *RouteMimeTable;

typedef status (*RouteFunc)(Buff *bf, Route *rt, Inst *data, HttpCtx *ctx);

Route *Route_Make(MemCh *m);
Route *Route_From(MemCh *m, StrVec *dir);
status Route_SetTargetFile(Route *rt, Str *ext, Str *absPath);
status Route_Handle(Route *rt, Span *dest, Inst *data, HttpCtx *ctx);
status Route_Prepare(Route *rt);
Route *Route_Get(Route *rt, StrVec *path);
Single *Route_MimeFunc(StrVec *path);

status Route_CheckEtag(Route *rt, StrVec *etag);
status Route_SetEtag(Route *rt, Str *path, struct timespec *mod);
status Route_CollectConfig(Route *root, StrVec *name, StrVec *path, Table *configAtts);

status Route_Init(MemCh *m);
