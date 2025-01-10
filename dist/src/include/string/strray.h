/*
typedef struct strray_slab {
    Type type;
    int length;
    byte bytes[STRING_CHUNK_SIZE];
    i64 idx;
    size_t size;
    int stride; 
} StrraySlab;

typedef struct strray {
    Type type;
    Span *indicies;
} Strray;

Strray *Strray_Make(MemCtx *m, ssize_t sz);
Strray *Strray_Add(Strray *srr, void *v);
Strray *Strray_Set(Strray *srr, i64 idx, void *v);
status Strray_Get(Strray *srr, i64 idx, void **v);
*/
