typedef Inst Route;

typedef struct route_ctx {
    Type type;
    MemCh *m;
    Route *root;
    StrVec *path;
} RouteCtx;

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
    ROUTE_PROPIDX_FILE = 3,
    ROUTE_PROPIDX_FUNC = 4,
    ROUTE_PROPIDX_MIME = 5,
    ROUTE_PROPIDX_TYPE = 6,
    ROUTE_PROPIDX_ACTION = 7,
    ROUTE_PROPIDX_HEADERS = 8,
    ROUTE_PROPIDX_ADD_STEP = 9,
};

extern struct span *RouteFuncTable;
extern struct span *RouteMimeTable;

typedef status (*RouteFunc)(Buff *bf, void *action, Inst *data, void *source);

Route *Route_Make(MemCh *m);
Route *Route_From(MemCh *m, StrVec *dir);
status Route_Collect(Route *rt, StrVec *path);
status Route_SetTargetFile(Route *rt, Str *ext, Str *absPath);
status Route_Handle(Route *rt, Buff *bf, Inst *data, void *source);
status Route_Prepare(Route *rt, RouteCtx *ctx);
Route *Route_GetHandler(Route *rt, StrVec *path);
Single *Route_MimeFunc(StrVec *path);

status Route_CheckEtag(Route *rt, StrVec *etag);
status Route_SetEtag(Route *rt, Str *path, struct timespec *mod);

status Route_ClsInit(MemCh *m);
