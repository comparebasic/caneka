typedef struct pfd_arr {
    Type type;
    i32 length;
    struct pollfd *pfds;
} PfdArr;

PfdArr *PfdArr_Make(MemCh *m, i32 length);
