typedef struct single  {
    Type type;
    union {
        util value;
        Abstract *a;
        void *ptr;
        DoFunc dof;
    } val;
} Single;

Single *Single_Ptr(MemCh *m, void *ptr);
Single *Do_Wrapped(MemCh *m, DoFunc dof);
Single *Single_Clone(MemCh *m, Abstract *sg);
