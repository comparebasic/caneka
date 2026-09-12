enum object_flags {
    BUILDOBJ_SATISFIED = 1 << 9, /* same as BuildModule */
};

typedef struct build_object {
    Type type;
    StrVec *src;
    StrVec *dest;
    BuildModule *md;
} BuildObject;

BuildObject *BuildObject_Current(MemCh *m, BuildCtx *ctx);
void BuildObject_Build(MemCh *m, BuildCtx *ctx, BuildObject *obj);
StrVec *BuildObject_GetDest(MemCh *m, BuildCtx *ctx, BuildModule *md, StrVec *path);
