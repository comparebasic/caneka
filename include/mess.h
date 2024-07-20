typedef struct mess {
    Type type;
    union {
        Abstract *a;
        util i64;
        int i32;
        word i;
        byte b;
    } value;
    Span *children;
    struct mess *parent;
} Mess;
