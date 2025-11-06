#include <external.h>
#include <caneka.h>

static void setLastFlag(MemIter *mit){
    Abstract *a = MemIter_Get(mit);
    i64 sz = 0;
    if(a != NULL){
        if(a->type.of > _TYPE_RANGE_TYPE_START && a->type.of < _TYPE_RANGE_TYPE_END){
            sz = (i64)(((RangeType *)a)->range)+sizeof(RangeType);
        }else{
            sz = Lookup_GetRaw(SizeLookup, a->type.of);
        }
    }
    if(sz > 0 && mit->ptr+sz-1 == mit->end){
        mit->type.state |= LAST; 
    }
}

Abstract *MemIter_Get(MemIter *mit){
    if(mit->end != NULL && mit->ptr != NULL){
        return (Abstract *)mit->ptr;
    }
    return NULL;
}

status MemIter_Next(MemIter *mit){
    mit->type.state &= ~LAST;
    Abstract *args[5];
    if(mit->ptr == NULL && (mit->type.state & (MORE|PROCESSING)) == MORE){
        MemPage *pg = NULL;
        if(mit->type.state & MEM_ITER_STREAM){
            pg = (MemPage *)mit->input.arr[mit->slIdx];
        }else{
            pg = (MemPage *)Span_Get(mit->input.target->it.p, mit->slIdx);
        }
        if(pg == NULL){
            Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error: unable to find page", args);
            mit->type.state |= ERROR;
            return mit->type.state;
        }
        mit->ptr = ((void *)pg)+sizeof(MemPage)+((util)pg->remaining);
        mit->end = ((void *)pg) + PAGE_SIZE-1;
        mit->type.state |= PROCESSING;
        setLastFlag(mit);
    }else if((mit->type.state & (MORE|PROCESSING)) == (MORE|PROCESSING)){
        mit->type.state &= ~MORE;
    }else{
        Abstract *a = MemIter_Get(mit);
        i64 sz = 0;
        if(a->type.of > _TYPE_RANGE_TYPE_START && a->type.of < _TYPE_RANGE_TYPE_END){
            sz = (i64)(((RangeType *)a)->range)+sizeof(RangeType);
        }else{
            sz = Lookup_GetRaw(SizeLookup, a->type.of);
        }
        if(sz <= 0){
            Abstract *args[] = {
                (Abstract *)Type_ToStr(ErrStream->m, a->type.of),
                NULL
            };
            Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error: type $ of object does not have a registered size", args);
            mit->type.state |= (ERROR|END);
        }
        if(mit->end == NULL || (void *)(mit->ptr+sz-1) > mit->end){
            args[0] = (Abstract *)Util_Wrapped(ErrStream->m, mit->ptr+sz-1 - mit->end);
            args[1] = NULL;
            Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error: type to large to increment address is off the page by $", args);
        }
        if(mit->ptr+sz-1 == mit->end){
            if(mit->slIdx < mit->maxSlIdx){
                mit->type.state = (mit->type.state & UPPER_FLAGS) | MORE;
                mit->ptr = mit->end = NULL;
                mit->slIdx++;
                if(mit->slIdx == mit->maxSlIdx){
                    mit->type.state |= LAST;
                }
            }else{
                mit->type.state |= END;
            }
        }else{
            mit->ptr += sz;
            setLastFlag(mit);
            mit->type.state |= PROCESSING;
        }
    }
    return mit->type.state;
}

void MemIter_Init(MemIter *mit, MemCh *target){
    memset(mit, 0, sizeof(MemIter));
    mit->type.of = TYPE_MEM_ITER;
    mit->input.target = target;
    mit->slIdx = 0;
    mit->type.state = MORE;
    mit->ptr = NULL;
    mit->end = NULL;
    mit->maxSlIdx = target->it.p->max_idx;
}

void MemIter_InitArr(MemIter *mit, Abstract **arr, i32 maxSlIdx){
    memset(mit, 0, sizeof(MemIter));
    mit->type.of = TYPE_MEM_ITER;
    mit->input.arr = arr;
    mit->slIdx = 0;
    mit->maxSlIdx = maxSlIdx;
    mit->type.state = MORE|MEM_ITER_STREAM;
    mit->ptr = NULL;
    mit->end = NULL;
}

MemIter *MemIter_Make(MemCh *m, MemCh *target){
    MemIter *mit = (MemIter *)MemCh_AllocOf(m, sizeof(MemIter), TYPE_MEM_ITER);
    mit->type.of = TYPE_MEM_ITER;
    mit->type.state = MORE;
    mit->input.target = target;
    mit->maxSlIdx = target->it.p->max_idx;
    return mit;
}

MemIter *MemIter_MakeFromArr(MemCh *m, Abstract **arr, i32 maxSlIdx){
    MemIter *mit = (MemIter *)MemCh_AllocOf(m, sizeof(MemIter), TYPE_MEM_ITER);
    mit->type.of = TYPE_MEM_ITER;
    mit->type.state = MORE|MEM_ITER_STREAM;
    mit->input.arr = arr;
    mit->maxSlIdx = maxSlIdx;
    return mit;
}
