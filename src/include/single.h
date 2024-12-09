typedef struct single  {
    Type type;
    union {
        util value;
        Abstract *a;
        void *ptr;
        DoFunc dof;
    } val;
} Single;

Single *Single_Ptr(MemCtx *m, void *ptr);
Single *Do_Wrapped(MemCtx *m, DoFunc dof);
Single *Single_Clone(MemCtx *m, Abstract *sg);
