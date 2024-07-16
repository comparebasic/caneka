typedef struct mess {
    Type type;
    union {
        Abstrac *a;
        util i64
        int i32;
        word i;
        byte b;
    } value;
    Span *children;
    Mess *parent;
} Mess;
