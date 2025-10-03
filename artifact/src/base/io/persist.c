#include <external.h>
#include <caneka.h>

static boolean _initialized = FALSE;
struct lookup *BlankerLookup = NULL;
struct lookup *RepointerLookup = NULL;

cls Persist_UnpackAddr(PersistCoord *coord, Abstract **arr){
    cls typeOf = coord->typeOf;
    MemPage *pg = (MemPage *)arr[coord->idx];
    if(pg == NULL){
        Error(ErrStream->m, NULL, FUNCNAME, FILENAME, LINENUMBER,
            "Cannot unpack address onto empty page", NULL);
    }
    util u = (util)pg;
    u |= coord->offset;
    void *ptr = (void *)coord;
    memcpy(ptr, &u, sizeof(void *));
    return typeOf;
}

status Persist_PackAddr(cls typeOf, i32 slIdx, void **ptr){
    util u = (util)*ptr;
    u &= MEM_PERSIST_MASK;
    PersistCoord coord = {
       .typeOf = typeOf,
       .idx = slIdx,
       .offset = (quad)u,
    };
    memcpy(ptr, &coord, sizeof(void *));
    return SUCCESS;
}

i16 Persist_PackMemCh(MemCh *m, MemIter *mit, Table *tbl, MemCh **persist){
    boolean pack = (mit->type.state & MEM_ITER_STREAM) == 0;
    Abstract *args[5];
    i16 checksum = 0;
    while((MemIter_Next(mit) & END) == 0){
        Abstract *a = MemIter_Get(mit);
        if((mit->type.state & MORE) == 0){
            if(a->type.of == TYPE_POINTER_ARRAY){
                void **dptr = (void **)(((void *)a)+sizeof(RangeType));
                i32 slots = ((RangeType *)a)->range / sizeof(void *);
                void **end = dptr+slots-1;
                while(dptr <= end){
                    void *ptr = *dptr;
                    if(ptr == NULL){
                        break;
                    }
                    if(pack){
                        PersistItem *item = (PersistItem *)Table_Get(tbl, 
                            (Abstract *)Util_Wrapped(m, (util)ptr));
                        if(item != NULL){
                            Persist_PackAddr(item->coord.typeOf,
                                item->coord.idx, (void **)dptr);

                            PersistCoord *coord = (PersistCoord *)dptr;
                        }else{
                            args[0] = (Abstract *)Util_Wrapped(m, (util)ptr);
                            args[1] = NULL;
                            Error(ErrStream->m, (Abstract *)mit, 
                                FUNCNAME, FILENAME, LINENUMBER,
                                "PTR ARRAY Unable to find address @ in table,"
                                " may be external to this MemCh", args);
                        }
                    }else{
                        PersistCoord *coord = (PersistCoord *)dptr;
                        Persist_UnpackAddr(coord, mit->input.arr);
                    }
                    checksum++;
                    dptr++;
                }
            }else if(a->type.of > _TYPE_ABSTRACT_BEGIN){
                Map *map = (Map *)Lookup_Get(MapsLookup, a->type.of);
                if(map == NULL){
                    args[0] = (Abstract *)Type_ToStr(m, a->type.of);
                    args[1] = NULL;
                    Error(ErrStream->m, (Abstract *)mit, 
                        FUNCNAME, FILENAME, LINENUMBER,
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
                        Abstract **aa = ((void *)a)+att->range;
                        if(pack){
                            PersistItem *item = (PersistItem *)Table_Get(tbl, 
                                (Abstract *)Util_Wrapped(m, (util)*aa));
                            if(item != NULL){
                                Persist_PackAddr(item->coord.typeOf,
                                    item->coord.idx, (void **)aa);
                                PersistCoord *coord = (PersistCoord *)aa;
                            }else{
                                args[0] = (Abstract *)Util_Wrapped(m, (util)aa);
                                args[1] = (Abstract *)map->keys[i];
                                args[2] = NULL;
                                Error(ErrStream->m, (Abstract *)mit,
                                    FUNCNAME, FILENAME, LINENUMBER,
                                    "Att Unable to find address $ for $ in table,"
                                    " may be external to this MemCh", args);
                            }
                        }else{
                            PersistCoord *coord = (PersistCoord *)aa;
                            Persist_UnpackAddr(coord, mit->input.arr);
                        }
                        checksum++;
                    }
                }
                if(!pack && a->type.of == TYPE_MEMCTX){
                    *persist = (MemCh *)a;
                }
            }
        }
    }

    return checksum;
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
            void *ptr = (void *)a;
            if(a->type.of < _TYPE_RANGE_TYPE_END){
                void *orig = ptr;
                ptr += sizeof(RangeType);
            }

            Table_Set(tbl, (Abstract *)Util_Wrapped(m, (util)ptr), 
                (Abstract *)PersistItem_Make(m, mit.slIdx, (void *)ptr, a->type.of));
        }
    }

    Span *pages = Span_Make(m);
    Iter_Reset(&persist->it);
    while((Iter_Next(&persist->it) & END) == 0){
        Abstract *a = Iter_Get(&persist->it);
        Table_Set(tbl, (Abstract *)Util_Wrapped(m, (util)a), 
            (Abstract *)PersistItem_Make(m, persist->it.idx, (void *)a, a->type.of));
        Span_Add(pages, a);
    }

    MemIter_Init(&mit, persist);
    i16 checksum = Persist_PackMemCh(m, &mit, tbl, NULL);

    PersistHeader hdr = {
        .pages = (i16)pages->nvalues,
        .checksum = checksum
    };
    Stream_Bytes(sm, (byte *)&hdr, sizeof(PersistHeader));

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
            pages = (Abstract **)Bytes_Alloc(sm->m, hdr.pages*sizeof(void *), TYPE_POINTER_ARRAY);
        }

        if(count >= hdr.pages){
            r |= SUCCESS;
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
        MemIter mit;
        MemIter_InitArr(&mit, pages, hdr.pages-1);
        checksum = Persist_PackMemCh(m, &mit, NULL, &persist);
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
    
    return persist;
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
