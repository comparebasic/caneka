#include <external.h>
#include <caneka.h>

static boolean _initialized = FALSE;
struct lookup *BlankerLookup = NULL;
struct lookup *RepointerLookup = NULL;

static cls Persist_UnpackAddr(PersistCoord *coord, Abstract **arr){
    cls typeOf = coord->typeOf;
    MemPage *pg = (MemPage *)arr[coord->idx];
    util u = (util)pg;
    u &= ~MEM_PERSIST_MASK;
    u |= coord->offset;
    void *ptr = (void *)coord;
    memcpy(ptr, &u, sizeof(void *));
    return typeOf;
}

static status Persist_PackAddr(cls typeOf, i32 slIdx, void **ptr){
    util u = (util)ptr;
    u &= ~MEM_PERSIST_MASK;

    PersistCoord coord = {
       .typeOf = typeOf,
       .idx = slIdx,
       .offset = (quad)u,
    };
    memcpy(ptr, &coord, sizeof(void *));
    return SUCCESS;
}

status Persist_FlushFree(Stream *sm, MemCh *persist){
    status r = READY;
    SourceFunc func = NULL;
    Abstract *a = NULL;
    Abstract *args[5];
    Iter it;

    MemCh *m = sm->m;
    Table *tbl = Table_Make(m);

    MemIter mit;
    MemIter_Init(&mit, persist);
    while((MemIter_Next(&mit) & END) == 0){
        Abstract *a = MemIter_Get(&mit);
        if((mit.type.state & MORE) == 0){
            Table_Set(tbl, (Abstract *)Util_Wrapped(m, (util)a), 
                (Abstract *)PersistItem_Make(m, mit.slIdx, (void *)a, a->type.of));
        }
    }

    Span *pages = Span_Make(m);
    Iter_Reset(&persist->it);
    while((Iter_Next(&persist->it) & END) == 0){
        Abstract *a = Iter_Get(&persist->it);
        Table_Set(tbl, (Abstract *)Util_Wrapped(m, (util)a), 
            (Abstract *)PersistItem_Make(m, it.idx, (void *)a, a->type.of));
        Span_Add(pages, a);
    }

    i16 checksum = 0;
    MemIter_Init(&mit, persist);
    while((MemIter_Next(&mit) & END) == 0){
        Abstract *a = MemIter_Get(&mit);
        if((mit.type.state & MORE) == 0){
            args[0] = (Abstract *)Type_ToStr(m, a->type.of);
            args[1] = NULL;
            Out("^c.Packing Item $^0.\n", args);
            if(a->type.of == TYPE_POINTER_ARRAY){
                void **dptr = (void **)(((void *)a)+sizeof(RangeType));
                i32 slots = ((RangeType *)a)->range / sizeof(void *);
                void **end = dptr+slots-1;
                while(dptr <= end){
                    void *ptr = *dptr;
                    if(ptr == NULL){
                        break;
                    }
                    PersistItem *item = (PersistItem *)Table_Get(tbl, 
                        (Abstract *)Util_Wrapped(m, (util)ptr));
                    if(item != NULL){
                        printf("   item ptr\n");
                        fflush(stdout);
                        Persist_PackAddr(item->coord.typeOf, item->coord.idx, (void **)dptr);
                        checksum++;
                    }else{
                        args[0] = (Abstract *)Util_Wrapped(m, (util)ptr);
                        args[1] = NULL;
                        Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                            "Unable to find address @ in table, may be external to this MemCh", args);
                    }
                    dptr++;
                }
            }else if(a->type.of > _TYPE_ABSTRACT_BEGIN){
                if(a->type.of == TYPE_MEMCTX){
                    printf("MemCtx handling found\n");
                    exit(1);
                    continue;
                }else{
                    Map *map = (Map *)Lookup_Get(MapsLookup, a->type.of);
                    if(map == NULL){
                        args[0] = (Abstract *)Type_ToStr(m, a->type.of);
                        args[1] = NULL;
                        Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                            "Map not found for type $, needed for mem persist", args);
                        return ERROR;
                    }
                    if(a->type.of == TYPE_ITER){
                        Iter *itp = (Iter *)a;
                        Iter_Init(itp, itp->p);
                    }
                    for(i16 i = 1; i <= map->type.range; i++){
                        RangeType *att = map->atts+i;
                        if(att->of > _TYPE_RANGE_TYPE_START){
                            Abstract *aa = ((void *)a)+att->range;
                            PersistItem *item = (PersistItem *)Table_Get(tbl, 
                                (Abstract *)Util_Wrapped(m, (util)a));
                            if(item != NULL){
                                printf("   item attr\n");
                                fflush(stdout);
                                Persist_PackAddr(item->coord.typeOf, item->coord.idx, (void **)&aa);
                                checksum++;
                            }else{
                                Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                                    "Unable to find address in table, may be external to this MemCh", NULL);
                            }
                        }
                    }
                }
            }
        }
    }

    PersistHeader hdr = {
        .pages = (i16)pages->nvalues,
        .checksum = checksum
    };
    Stream_Bytes(sm, (byte *)&hdr, sizeof(PersistHeader));

    args[0] = (Abstract *)I16_Wrapped(OutStream->m, hdr.pages);
    args[1] = (Abstract *)I16_Wrapped(OutStream->m, hdr.checksum);
    args[2] = NULL;
    Out("^y.Wrote header ^D.$^d.pages and ^D.$^d.changes^0.\n", args);

    Iter_Init(&it, pages);
    while((Iter_Next(&it) & END) == 0){
        MemPage *pg = (MemPage *)Iter_Get(&it);
        if(Stream_Bytes(sm, (byte *)pg, PAGE_SIZE) != PAGE_SIZE){
            Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                "Error writing page to stream for Persist", NULL);
            r |= ERROR;
            break;
        }
        r |= MemBook_FreePage(m, (MemPage *)Iter_Get(&it));
    }

    return r;
}

MemCh *Persist_FromStream(Stream *sm){
    status r = READY;
    Abstract *args[5];
    PersistHeader hdr = {0, 0};
    i16 count = 0;
    MemCh *persist = NULL; 
    Abstract **pages = NULL;
    MemCh *m = sm->m;
    while((r & (SUCCESS|ERROR)) == 0 && (sm->type.state & END) == 0){
        if((r & PROCESSING) == 0){
            if(Stream_ReadToMem(sm, sizeof(PersistHeader), (byte *)&hdr) != sizeof(PersistHeader)){
                r |= ERROR;
                break;
            }
            r |= PROCESSING;
            args[0] = (Abstract *)I16_Wrapped(OutStream->m, hdr.pages);
            args[1] = (Abstract *)I16_Wrapped(OutStream->m, hdr.checksum);
            args[2] = NULL;
            Out("^y.Found header ^D.$^d.pages and ^D.$^d.changes^0.\n", args);
            pages = (Abstract **)Bytes_Alloc(sm->m, hdr.pages*sizeof(void *), TYPE_POINTER_ARRAY);
        }

        if(count >= hdr.pages){
            r |= SUCCESS;
            args[0] = (Abstract *)I16_Wrapped(OutStream->m, hdr.pages);
            args[1] = (Abstract *)I16_Wrapped(OutStream->m, count);
            args[2] = NULL;
            Out("^y.Done! have ^D.$^d.count of ^D.$^d.pages^0.\n", args);
            break;
        }

        pages[count] = MemBook_GetPage(NULL);
        if(pages[count] == NULL){
            Fatal(FUNCNAME, FILENAME, LINENUMBER,
                "Error allocating page", NULL);
            r |= ERROR;
        }

        i64 length = Stream_ReadToMem(sm, PAGE_SIZE, (byte *)pages[count]);
        if(length != PAGE_SIZE){
            args[0] = (Abstract *)I64_Wrapped(ErrStream->m, length);
            args[1] = NULL;
            Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                "Error reading page from stream to Persist length $", args);
            r |= ERROR;
        }
        count++;
    }

    i16 checksum = 0;

    if(r & SUCCESS){
        r &= ~SUCCESS;
        /* do replace adder thing here */
        MemIter mit;
        MemIter_InitArr(&mit, pages, hdr.pages-1);
        while((MemIter_Next(&mit) & END) == 0){
            Abstract *a = MemIter_Get(&mit);
            if((mit.type.state & MORE) == 0){
                args[0] = (Abstract *)Type_ToStr(m, a->type.of);
                args[1] = NULL;
                Out("^c.Item $^0.\n", args);
                if(a->type.of == TYPE_POINTER_ARRAY){
                    void **dptr = (void **)(((void *)a)+sizeof(RangeType));
                    i32 slots = ((RangeType *)a)->range / sizeof(void *);
                    void **end = dptr+slots-1;
                    while(dptr <= end){
                        void *ptr = *dptr;
                        if(ptr != NULL){
                            checksum++;
                            printf("   item ptr\n");
                            fflush(stdout);
                        }
                        Persist_UnpackAddr((PersistCoord *)&ptr, mit.input.arr);
                        dptr++;
                    }
                }else if(a->type.of > _TYPE_ABSTRACT_BEGIN){
                    if(a->type.of == TYPE_MEMCTX){
                        printf("MemCtx handling found\n");
                        exit(1);
                        continue;
                    }
                    Map *map = (Map *)Lookup_Get(MapsLookup, a->type.of);
                    if(map == NULL){
                        args[0] = (Abstract *)Type_ToStr(m, a->type.of);
                        args[1] = NULL;
                        Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                            "Map not found for type $, needed for mem persist", args);
                        r |= ERROR;
                        break;
                    }
                    if(a->type.of == TYPE_ITER){
                        Iter *itp = (Iter *)a;
                        Iter_Init(itp, itp->p);
                    }
                    for(i16 i = 1; i <= map->type.range; i++){
                        RangeType *att = map->atts+i;
                        if(att->of > _TYPE_RANGE_TYPE_START){
                            void *aa = ((void *)a)+att->range;
                            Persist_UnpackAddr((PersistCoord *)&aa, mit.input.arr);
                            checksum++;
                            printf("   item attr\n");
                            fflush(stdout);
                        }
                    }
                }
            }
        }
    }

    if(checksum != hdr.checksum){
        args[0] = (Abstract *)I16_Wrapped(sm->m, hdr.checksum);
        args[1] = (Abstract *)I16_Wrapped(sm->m, checksum);
        args[2] = NULL;
        Error(ErrStream->m, (Abstract *)sm, FUNCNAME, FILENAME, LINENUMBER,
            "Error checksum of number of changes does not match for resurecting persisting"             " expected $, have $",
            args);
        r |= ERROR;
    }else{
        r |= SUCCESS;
    }

    if((r & (SUCCESS|ERROR)) == SUCCESS){
        return persist;
    }
    
    return NULL;
}

status Persist_Init(MemCh *m){
    status r = READY;
    if(!_initialized){
        _initialized = TRUE;
        BlankerLookup = Lookup_Make(m, 0);
        RepointerLookup = Lookup_Make(m, 0);
        r |= SUCCESS;
    }

    if(r == READY){
        r = NOOP;
    }

    return r;
}
