#define STR_MAX (MEM_SLAB_SIZE-1)
typedef struct str {
    Type type;
    int length;
    byte *bytes;
    struct str *next;
} Str;

Str *Str_Make(MemCtx *m, byte *ptr, i64 length);
