typedef struct result {
    Type type;
    int range;
    String *s;
    Abstract *source;
} Result;

Result *Result_Make(MemCtx *m, int range, String *s, Abstract *source);
