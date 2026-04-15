typedef struct route_gen_type {
    Type type;
    RouteFunc func;
} RouteGenType;

typedef route_gen {
    Type type;
    StrVec *path;
    Table *headers;
    Templ *templ;
    Str *mime;
    Str *ext;
    Abstract *action;
    TaskFunc taskFunc;
    RouteGenType *gen;
} RouteGen;

RouteGen *RouteGen_Make(MemCh *m);
