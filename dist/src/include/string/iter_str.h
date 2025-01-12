typedef struct iter_string {
    Type type;
    int idx;
    size_t sz;
    Cursor cursor;
    i64 count;
} IterStr;

status IterStr_Next(IterStr *it);
Abstract *IterStr_Get(IterStr *it);
IterStr *IterStr_Make(MemCtx *m, String *s, size_t sz);
status IterStr_Init(IterStr *it, String *s, size_t sz);
