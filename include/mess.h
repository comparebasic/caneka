typedef struct mess {
    Type type;
    union {
        Abstract *a;
        void *ptr;
        util i64;
        int i32;
        word w;
        byte b;
    } value;
    struct mess *next;
    struct mess *parent;
} Mess;
