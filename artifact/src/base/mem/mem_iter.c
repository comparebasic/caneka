#include <external.h>
#include <caneka.h>

static void setLastFlag(MemIter *mit){
    Abstract *a = MemIter_Get(mit);
    i64 sz = 0;
    if(a != NULL){
        if(a->type.of > _TYPE_RANGE_TYPE_START && a->type.of < _TYPE_RANGE_TYPE_END){
            if(a->type.of == TYPE_BYTES_POINTER){
                sz = (i64)(((RangeType *)a)->range)+sizeof(RangeType);
            }else if(a->type.of == TYPE_POINTER_ARRAY){
                sz = (i64)(((RangeType *)a)->range*sizeof(void *))+sizeof(RangeType);
            }else if(a->type.of == TYPE_RANGE_ARRAY){
                sz = (i64)(((RangeType *)a)->range*sizeof(RangeType))+sizeof(RangeType);
            }
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
    if(mit->ptr == NULL && (mit->type.state & (MORE|PROCESSING)) == MORE){
        MemPage *pg = (MemPage *)Span_Get(mit->target->it.p, mit->slIdx);
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
            if(a->type.of == TYPE_BYTES_POINTER){
                sz = (i64)(((RangeType *)a)->range)+sizeof(RangeType);
            }else if(a->type.of == TYPE_POINTER_ARRAY){
                sz = (i64)(((RangeType *)a)->range*sizeof(void *))+sizeof(RangeType);
            }else if(a->type.of == TYPE_RANGE_ARRAY){
                sz = (i64)(((RangeType *)a)->range*sizeof(RangeType))+sizeof(RangeType);
            }
        }else{
            sz = Lookup_GetRaw(SizeLookup, a->type.of);
        }
        if(sz <= 0){
            Abstract *args[] = {
                (Abstract *)Type_ToStr(ErrStream->m, a->type.of),
                NULL
            };
            Error(ErrStream->m, (Abstract *)mit, FUNCNAME, FILENAME, LINENUMBER,
                "Error: type $ of object does not have a registered size", args);
            mit->type.state |= (ERROR|END);
        }
        if(mit->end == NULL || (void *)(mit->ptr+sz-1) > mit->end){
            Error(ErrStream->m, (Abstract *)mit, FUNCNAME, FILENAME, LINENUMBER,
                "Error: type to large to increment address is off the page", NULL);
        }
        if(mit->ptr+sz-1 == mit->end){
            if(mit->slIdx < mit->target->it.p->max_idx){
                mit->type.state = MORE;
                mit->ptr = mit->end = NULL;
                mit->slIdx++;
                if(mit->slIdx == mit->target->it.p->max_idx){
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
    mit->type.of = TYPE_MEM_ITER;
    mit->target = target;
    mit->slIdx = 0;
    mit->type.state = MORE;
    mit->ptr = NULL;
    mit->end = NULL;
}

MemIter *MemIter_Make(MemCh *m, MemCh *target){
    MemIter *mit = (MemIter *)MemCh_AllocOf(m, sizeof(MemIter), TYPE_MEM_ITER);
    mit->type.of = TYPE_MEM_ITER;
    mit->type.state = MORE;
    mit->target = target;
    return mit;
}
