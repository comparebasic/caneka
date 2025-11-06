typedef Object Route;

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
    ROUTE_PAGE = 1 << 13,
    ROUTE_FILEDB = 1 << 11,
    ROUTE_ASSET = 1 << 12,
    ROUTE_SCRIPT = 1 << 14,
    ROUTE_FORBIDDEN = 1 << 15,
};

enum route_prop_idx {
    ROUTE_PROPIDX_PATH = 0,
    ROUTE_PROPIDX_FILE = 1,
    ROUTE_PROPIDX_FUNC = 2,
    ROUTE_PROPIDX_MIME = 3,
    ROUTE_PROPIDX_TYPE = 4,
    ROUTE_PROPIDX_ACTION = 5,
    ROUTE_PROPIDX_DATA = 6,
};

extern struct span *RouteFuncTable;
extern struct span *RouteMimeTable;

typedef status (*RouteFunc)(Buff *bf, Abstract *action, Object *data, Abstract *source);

Route *Route_Make(MemCh *m);
status Route_Collect(Route *rt, StrVec *path);
status Route_SetTargetFile(Route *rt, Str *ext, Str *absPath);
status Route_Handle(Route *rt, Buff *bf, Object *data, Abstract *source);
status Route_Prepare(Route *rt, RouteCtx *ctx);

status Route_ClsInit(MemCh *m);
