enum object_flags {
    BUILDOBJ_SATISFIED = 1 << 9, /* same as BuildModule */
    BUILDOBJ_EXEC = 1 << 15,
};

typedef struct build_object {
    Type type;
    StrVec *src;
    StrVec *dest;
    BuildModule *md;
} BuildObject;

BuildObject *BuildObject_Current(MemCh *m, BuildCtx *ctx);
BuildObject *BuildObject_Exec(MemCh *m, BuildCtx *ctx, BuildModule *md, StrVec *path);
void BuildObject_Build(MemCh *m, BuildCtx *ctx, BuildObject *obj);
void BuildObject_Link(MemCh *m, BuildCtx *ctx, BuildObject *obj);
StrVec *BuildObject_GetDest(MemCh *m, BuildCtx *ctx, BuildModule *md, StrVec *path);
