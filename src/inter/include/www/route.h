typedef Inst Route;

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
