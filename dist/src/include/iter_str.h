typedef struct iter_string {
    Type type;
    int idx;
    size_t sz;
    byte *item;
    SCursor cursor;
    i64 count;
} IterStr;

status IterStr_Next(IterStr *it);
Abstract *IterStr_Get(IterStr *it);
status IterStr_Init(IterStr *it, String *s, size_t sz, byte *item);
IterStr *IterStr_Make(MemCtx *m);
