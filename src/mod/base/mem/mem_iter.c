#include <external.h>
#include "base_module.h"

static void setLastFlag(MemIter *mit){
    MemIdent *mi = MemIter_Get(mit);

    void *ar[] = {mi, NULL};
    Out("^p.Set last Flag@^0\n", ar);

    Abstract *a = mi->ptr;

    i64 sz = 0;
    if(a != NULL){
        if(a->type.of > _TYPE_RANGE_TYPE_START && a->type.of < _TYPE_RANGE_TYPE_END){
            sz = (i64)(((RangeType *)a)->range)+sizeof(RangeType);
        }else{
            IfcMap *imap = Lookup_Get(IfcLookup, a->type.of);
            if(imap == NULL){
                void *ar[] = {Type_ToStr(ErrStream->m, a->type.of), NULL};
                Error(mit->m, FUNCNAME, FILENAME, LINENUMBER,
                    "IfcMap not found for type @", ar);
                return;
            }
            sz = imap->size;
        }
    }
    if(sz > 0 && mit->current.ptr+sz-1 == mit->end){
        mit->type.state |= LAST; 
    }
}

MemIdent *MemIter_CloneCurrent(MemCh *m, MemIter *mit){
    MemIdent *mi = MemCh_AllocOf(m, sizeof(MemIdent), TYPE_MEM_IDENT);
    memcpy(mi, &mit->current, sizeof(MemIdent));
    return mi;
}

MemIdent *MemIter_Get(MemIter *mit){
    Debug_Push(mit->m, mit->current.ptr);
    if(mit->end != NULL && mit->current.ptr != NULL){
        Abstract *a = mit->current.ptr;
        if(a != NULL){
            mit->current.rtype.of = a->type.of;
            mit->current.idx++;
            if(a->type.of > _TYPE_RANGE_TYPE_START && a->type.of < _TYPE_RANGE_TYPE_END){
                byte *b = (byte *)a;
                mit->current.rtype.range = a->type.state;
                mit->current.content = b-sizeof(RangeType);
            }else{
                IfcMap *imap = Lookup_Get(IfcLookup, a->type.of);
                if(imap == NULL){
                    void *ar[] = {
                        a, NULL
                    };
                    Error(mit->m, FUNCNAME, FILENAME, LINENUMBER,
                        "imap map for interfaces not found MemIter_Get, @", ar);
                    Return(mit->m, NULL);
                }
                mit->current.rtype.range = imap->size;
                mit->current.content = a;
            }
        }else{
            mit->current.content = NULL;
        }

        void *ar[] = {&mit->current, NULL};
        Out("^y.Returning Current @^0\n", ar);

        Return(mit->m, &mit->current);
    }

    printf("NULL!\n");
    fflush(stdout);

    Return(mit->m, NULL);
}

status MemIter_Next(MemIter *mit){

    mit->type.state &= ~LAST;
    void *args[5];
    Debug_Push(mit->m, mit);

    printf("Next\n");
    fflush(stdout);

    if(mit->current.ptr == NULL && (mit->type.state & (MORE|PROCESSING)) == MORE){
        MemPage *pg = NULL;
        if(mit->type.state & MEM_ITER_STREAM){
            pg = (MemPage *)mit->input.arr[mit->current.slIdx];
        }else{
            pg = (MemPage *)Span_Get(mit->input.target->it.p, mit->current.slIdx);
        }
        if(pg == NULL){
            Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error: unable to find page", args);
            mit->type.state |= ERROR;

            Return(mit->m, mit->type.state);
        }
        mit->current.ptr = ((void *)pg)+sizeof(MemPage)+((util)pg->remaining);
        mit->end = ((void *)pg) + PAGE_SIZE-1;
        mit->type.state |= PROCESSING;
        setLastFlag(mit);
    }else if((mit->type.state & (MORE|PROCESSING)) == (MORE|PROCESSING)){
        mit->type.state &= ~MORE;
    }else{
        Abstract *a = (Abstract *)MemIter_Get(mit);
        i64 sz = 0;
        if(a->type.of > _TYPE_RANGE_TYPE_START && a->type.of < _TYPE_RANGE_TYPE_END){
            sz = (i64)(((RangeType *)a)->range)+sizeof(RangeType);
        }else{
            IfcMap *imap = Lookup_Get(IfcLookup, a->type.of);
            if(imap == NULL){
                void *ar[] = {Type_ToStr(mit->m, a->type.of), NULL};
                Error(mit->m, FUNCNAME, FILENAME, LINENUMBER,
                    "IfcMap not found for type @", ar);
                Return(mit->m, ERROR);
            }
            sz = imap->size;
        }
        if(sz <= 0){
            void *args[] = {
                Type_ToStr(ErrStream->m, a->type.of),
                NULL
            };
            Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error: type $ of object does not have a registered size", args);
            mit->type.state |= (ERROR|END);
        }
        if(mit->end == NULL || (void *)(mit->current.ptr+sz-1) > mit->end){
            args[0] = Util_Wrapped(ErrStream->m, mit->current.ptr+sz-1 - mit->end);
            args[1] = NULL;
            Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error: type to large to increment address is off the page by $", args);
        }
        if(mit->current.ptr+sz-1 == mit->end){
            if(mit->current.slIdx < mit->maxSlIdx){
                mit->type.state = (mit->type.state & UPPER_FLAGS) | MORE;
                mit->current.ptr = mit->end = NULL;
                mit->current.slIdx++;
                mit->current.idx = -1;
                if(mit->current.slIdx == mit->maxSlIdx){
                    mit->type.state |= LAST;
                }
            }else{
                mit->type.state |= END;
            }
        }else{
            mit->current.ptr += sz;
            setLastFlag(mit);
            mit->type.state |= PROCESSING;
        }
    }

    Return(mit->m, mit->type.state);
}

void MemIter_Init(MemCh *m, MemIter *mit, MemCh *target){
    memset(mit, 0, sizeof(MemIter));
    mit->type.of = TYPE_MEM_ITER;
    mit->m = m;
    mit->input.target = target;
    mit->current.slIdx = 0;
    mit->current.type.of = TYPE_MEM_IDENT;
    mit->current.idx = -1;
    mit->type.state = MORE;
    mit->current.ptr = NULL;
    mit->end = NULL;
    mit->maxSlIdx = target->it.p->max_idx;
}

void MemIter_InitArr(MemIter *mit, void **arr, i32 maxSlIdx){
    memset(mit, 0, sizeof(MemIter));
    mit->type.of = TYPE_MEM_ITER;
    mit->input.arr = arr;
    mit->current.slIdx = 0;
    mit->current.type.of = TYPE_MEM_IDENT;
    mit->current.idx = -1;
    mit->maxSlIdx = maxSlIdx;
    mit->type.state = MORE|MEM_ITER_STREAM;
    mit->current.ptr = NULL;
    mit->end = NULL;
}

MemIter *MemIter_Make(MemCh *m, MemCh *target){
    MemIter *mit = (MemIter *)MemCh_AllocOf(m, sizeof(MemIter), TYPE_MEM_ITER);
    mit->m = m;
    mit->type.of = TYPE_MEM_ITER;
    mit->type.state = MORE;
    mit->input.target = target;
    mit->maxSlIdx = target->it.p->max_idx;
    mit->current.type.of = TYPE_MEM_IDENT;
    mit->current.idx = -1;
    return mit;
}

MemIter *MemIter_MakeFromArr(MemCh *m, void **arr, i32 maxSlIdx){
    MemIter *mit = (MemIter *)MemCh_AllocOf(m, sizeof(MemIter), TYPE_MEM_ITER);
    mit->m = m;
    mit->type.of = TYPE_MEM_ITER;
    mit->type.state = MORE|MEM_ITER_STREAM;
    mit->input.arr = arr;
    mit->maxSlIdx = maxSlIdx;
    mit->current.type.of = TYPE_MEM_IDENT;
    mit->current.idx = -1;
    return mit;
}
