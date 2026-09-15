enum object_flags {
    BUILDOBJ_SATISFIED = 1 << 9, /* same as BuildModule */
    BUILDOBJ_EXEC = 1 << 15,
};

typedef struct build_object {
    Type type;
    i32 idx;
    Str *src;
    Str *dest;
    Str *dir;
    ProcDets pd;
    BuildModule *md;
} BuildObject;

BuildObject *BuildObject_From(MemCh *m, BuildCtx *ctx, BuildModule *md, StrVec *path);
BuildObject *BuildObject_Exec(MemCh *m, BuildCtx *ctx, BuildModule *md, StrVec *path);
BuildObject *BuildObject_Inc(MemCh *m, BuildCtx *ctx, BuildModule *md);
void BuildObject_Build(MemCh *m, BuildCtx *ctx, BuildObject *obj);
void BuildObject_Link(MemCh *m, BuildCtx *ctx, BuildObject *obj);
Str *BuildObject_GetDest(MemCh *m, BuildCtx *ctx, BuildModule *md, StrVec *path);
