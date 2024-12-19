typedef struct result {
    Type type;
    int range;
    String *s;
    Abstract *source;
    struct result *next;
} Result;

Result *Result_Make(MemCtx *m, int range, String *s, Abstract *source);
