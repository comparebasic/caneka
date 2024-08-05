typedef struct single  {
    Type type;
    union {
        util value;
        Abstract *a;
        void *ptr;
    } val;
} Single;

Single * Single_Ptr(MemCtx *m, void *ptr);
