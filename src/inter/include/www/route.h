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

void Route_BuildRoute(Route *root, StrVec *name, StrVec *path, Table *configAtts);
void Route_CollectConfig(Route *root,
        StrVec *name, StrVec *path, Table *configAtts);
void Route_CheckGenEtag(Gen *gen, StrVec *etag, Table *headers);
void Route_SetGenEtag(MemCh *m, Gen *gen, struct timespec *mod, Table *headers);
void Route_Prepare(Route *rt);
void Route_Handle(Route *rt, Span *dest, Table *data, HttpCtx *ctx);
Route *Route_Get(Route *root, StrVec *path);

void Route_Init(MemCh *m);
