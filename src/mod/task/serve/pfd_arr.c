#include <external.h>
#include <caneka.h>

void *PfdSpan_AddPfd(Span *p, struct pollfd *new){
    struct pollfd *pfd = PfdSpan_GetNextPfd(p);
    memcpy(pfd, new, sizeof(struct pollfd));
    return pfd;
}

i32 PfdSpan_Poll(Span *p, i32 timeout){
    i32 available = 0;
    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        PfdArr *arr = Iter_Get(&it);
        available += poll(arr->pfds, arr->length, timeout);
    }
    return available;
}

struct pollfd *PfdSpan_GetNextPfd(Span *p){
    PfdArr *arr = NULL;
    if(p->nvalues > 0){
        arr = Span_Get(p, p->max_idx);
    }

    if(arr == NULL || arr->length == SPAN_STRIDE){
        arr = PfdArr_Make(p->m, SPAN_STRIDE);
        Span_Add(p, arr);
    }

    struct pollfd *pfd = arr->pfds+arr->length;
    arr->length++;

    return pfd;
}

PfdArr *PfdArr_Make(MemCh *m, i16 alloc){
    PfdArr *arr = MemCh_AllocOf(m, sizeof(PfdArr), TYPE_POLL_FD_ARR);
    arr->pfds = (struct pollfd *)Bytes_Alloc(m,
        (word)(sizeof(struct pollfd)*alloc), TYPE_BYTE);

    struct pollfd *pfd = (struct pollfd *)arr->pfds;
    for(i32 i = 0; i < alloc; i++){
        pfd->fd = -1;
        pfd++;
    }

    arr->alloc = alloc;
    return arr;
}
