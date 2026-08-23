typedef struct pfd_arr {
    Type type;
    i16 length;
    i16 alloc;
    struct pollfd *pfds;
} PfdArr;

PfdArr *PfdArr_Make(MemCh *m, i16 alloc);

void *PfdSpan_AddPfd(Span *p, struct pollfd *pfd);
struct pollfd *PfdSpan_GetNextPfd(Span *p);
i32 PfdSpan_Poll(Span *p, i32 timeout);
