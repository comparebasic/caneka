#include <external.h>
#include <caneka.h>

i32 _increments[SPAN_MAX_DIMS+1] = {1, 16, 256, 4096, 65536, 1048576};

static inline i32 Iter_SetStack(Iter *it, i8 dim, i32 offset){
    Span *p = it->span; 
    void **ptr = NULL;
    void *debug = ptr;
    i32 localIdx = 0;
    i32 increment = _increments[dim];
    localIdx = (offset / increment);
    if(dim == p->dims){
        ptr = (void **)p->root;
        it->stackIdx[dim] = 0;
    }else{
        ptr = it->stack[dim+1]; 
        /*
        printf("setting pointer to %lu\n", (util)ptr);
        */
        it->stackIdx[dim] = localIdx;
    }

    debug = ptr;
    ptr += localIdx;
    it->stack[dim] = ptr;

    if(localIdx > SPAN_STRIDE){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error localIdx larger than span stride _i4", localIdx);
        return -1;
    }

    /*
    printf("set stack op-set:%d for idx:%d dim:%d offset:%d localIdx:%d slab:*%lu value:*%lu\n",
            (i32)((it->type.state & UPPER_FLAGS) == SPAN_OP_SET), it->idx, (i32)dim, offset, localIdx, (util)debug, (util)*ptr);

    */
    return offset % increment;
}

status Iter_Next(Iter *it){
    return it->type.state;
}

Abstract *Iter_Get(Iter *it){
    return Span_GetFromQ(it);
}

status Iter_Reset(Iter *it){
    it->type.state |= END;
    return SUCCESS;
}

void Iter_Init(Iter *it, Span *p){
    memset(it, 0, sizeof(Iter));
    return Iter_Setup(it, p, SPAN_OP_GET, 0);
}

void Iter_Setup(Iter *it, Span *p, status op, i32 idx){
    it->type.of = TYPE_SPAN_QUERY;
    it->type.state = op;
    it->span = p;
    it->idx = idx;
    it->metrics.get = it->metrics.set = it->metrics.selected = it->metrics.available = -1;
    return;
}
