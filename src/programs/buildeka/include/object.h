typedef struct build_object {
    Type type;
    StrVec *src;
    StrVec *dest;
    BuildModule *md;
} BuildObject;

BuildObject *BuildObject_Make(MemCh *m, BuildCtx *ctx, i32 modIdx, i32 idx);
