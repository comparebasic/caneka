#include <external.h>
#include "base_module.h"

static void setMemIdent(MemIter *mit, MemIdent *d){
    Abstract *a = mit->current.ptr;
    d->type.of = TYPE_MEM_IDENT;
    d->ptr = a;

    if(a != NULL){
        d->rtype.of = a->type.of;
        if(a->type.of > _TYPE_RANGE_TYPE_START && a->type.of < _TYPE_RANGE_TYPE_END){
            byte *b = (byte *)a;
            d->rtype.range = a->type.state;
            d->content = b+sizeof(RangeType);
        }else{
            IfcMap *imap = Lookup_Get(IfcLookup, a->type.of);
            if(imap == NULL){
                void *ar[] = {
                    a, NULL
                };
                Error(mit->m, FUNCNAME, FILENAME, LINENUMBER,
                    "imap map for interfaces not found MemIter_Get, @", ar);
                mit->type.state |= ERROR;
                return;
            }
            d->rtype.range = imap->size;
            d->content = a;
        }
    }else{
        d->rtype.of = ZERO;
        d->rtype.range = 0;
        d->content = NULL;
    }
}

static void setLastFlag(MemIter *mit){
    MemIdent *mi = MemIter_Get(mit);

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

MemIdent *MemIter_Get(MemIter *mit){
    Debug_Push(mit->m, mit->current.ptr);
    if(mit->end != NULL && mit->current.ptr != NULL){
        Return(mit->m, &mit->current);
    }
    Return(mit->m, NULL);
}

Table *MemIter_GetTable(MemCh *m, MemCh *target){
    MemIter mit;
    MemIter_Init(m, &mit, target);
    Table *tbl = Table_Make(m);
    i16 g = 0;
    i32 slIdx = 0;
    i32 idx = 0;
    while((MemIter_Next(&mit) & END) == 0){
        Guard_Incr(m, &g, 100, FUNCNAME, FILENAME, LINENUMBER);
        if(mit.type.state & MORE){
            slIdx++;
        }else{
            if((mit.type.state & MORE) == 0){
                MemIdent *mid = MemCh_AllocOf(m, sizeof(MemIdent), TYPE_MEM_IDENT);
                setMemIdent(&mit, mid);
                mid->idx = idx++;
                Single *key = Util_Wrapped(m, (util)mid->content);
                Table_Set(tbl, key, mid);
            }
        }
    }

    return tbl;
}

status MemIter_Next(MemIter *mit){
    MemCh *m = mit->m;

    mit->type.state &= ~LAST;
    void *args[5];
    Debug_Push(m, mit);

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

            Return(m, mit->type.state);
        }
        mit->current.ptr = ((void *)pg)+sizeof(MemPage)+((util)pg->remaining);
        mit->current.idx = 0;
        mit->end = ((void *)pg) + PAGE_SIZE-1;
        mit->type.state |= PROCESSING;
        setLastFlag(mit);
        setMemIdent(mit, &mit->current);
    }else if((mit->type.state & (MORE|PROCESSING)) == (MORE|PROCESSING)){
        mit->type.state &= ~MORE;
    }else{
        i64 sz = 0;
        Abstract *a = (Abstract *)mit->current.ptr;
        cls typeOf = a->type.of;

        if(typeOf > _TYPE_RANGE_TYPE_START && typeOf < _TYPE_RANGE_TYPE_END){
            sz = (i64)(((RangeType *)a)->range)+sizeof(RangeType);
        }else{
            a = (Abstract *)mit->current.content;
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
            memset(&mit->current, 0, sizeof(MemIdent));
            mit->current.type.of = TYPE_MEM_IDENT;
        }else{
            mit->type.state |= PROCESSING;
            mit->current.ptr += sz;
            mit->current.idx++;
            setLastFlag(mit);

            setMemIdent(mit, &mit->current);
        }
    }

    Return(mit->m, mit->type.state);
}

void MemIter_Init(MemCh *m, MemIter *mit, MemCh *target){
    memset(mit, 0, sizeof(MemIter));
    mit->type.of = TYPE_MEM_ITER;
    mit->type.state = MORE;
    mit->m = m;
    mit->input.target = target;
    mit->current.slIdx = 0;
    mit->current.type.of = TYPE_MEM_IDENT;
    mit->current.idx = -1;
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
