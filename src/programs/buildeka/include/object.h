typedef struct build_object {
    Type type;
    StrVec *src;
    StrVec *dest;
    BuildModule *md;
} BuildObject;

BuildObject *BuildObject_Current(MemCh *m, BuildCtx *ctx);
