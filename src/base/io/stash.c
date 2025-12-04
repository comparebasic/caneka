/* Base.io.Stash
 *
 * Block out the addresse of a MemCh memory context object in a predictible way
 * and then save/restore it from disk
 *
 * Because MemPage objects are always 4k, the relative address in the page
 * is always &4095. The page number as it appears in the MemCh is then
 * stored higher up on the otherwise zeroed out addres.
 *
 * MemIter is used to climb each page of a MemCh using the Map offsets
 * in the Lookup object setup by all the _map files for each type.
 *
 */

#include <external.h>
#include "base_module.h"

static boolean _initialized = FALSE;
struct lookup *BlankerLookup = NULL;
struct lookup *RepointerLookup = NULL;

cls Stash_UnpackAddr(MemCh *m, StashCoord *coord, void **arr){
    cls typeOf = coord->typeOf;
    MemPage *pg = (MemPage *)arr[coord->idx];
    if(pg == NULL){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Cannot unpack address onto empty page", NULL);
        return ZERO;
    }
    util u = (util)pg;
    u |= coord->offset;
    void *ptr = (void *)coord;
    memcpy(ptr, &u, sizeof(void *));
    return typeOf;
}

status Stash_PackAddr(cls typeOf, i32 slIdx, void **ptr){
    util u = (util)*ptr;
    u &= MEM_STASH_MASK;
    StashCoord coord = {
       .typeOf = typeOf,
       .idx = slIdx,
       .offset = (quad)u,
    };
    memcpy(ptr, &coord, sizeof(void *));
    return SUCCESS;
}

i16 Stash_PackMemCh(MemCh *m, MemIter *mit, Table *tbl, MemCh **persist){
    boolean pack = (mit->type.state & MEM_ITER_STREAM) == 0;
    void *args[5];
    i16 checksum = 0;
    i32 count = 0;
    while((MemIter_Next(mit) & END) == 0){
        Abstract *a = (Abstract *)MemIter_Get(mit);
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
                        StashItem *item = (StashItem *)Table_Get(tbl, 
                            Util_Wrapped(m, (util)ptr));
                        if(item != NULL){
                            Stash_PackAddr(item->coord.typeOf,
                                item->coord.idx, (void **)dptr);

                            StashCoord *coord = (StashCoord *)dptr;
                        }else{
                            args[0] = Util_Wrapped(m, (util)ptr);
                            args[1] = NULL;
                            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                                "PTR ARRAY Unable to find address @ in table,"
                                " may be external to this MemCh", args);
                        }
                    }else{
                        StashCoord *coord = (StashCoord *)dptr;
                        Stash_UnpackAddr(m, coord, mit->input.arr);
                    }
                    checksum++;
                    dptr++;
                }
            }else if(a->type.of > _TYPE_ABSTRACT_BEGIN){
                Map *map = (Map *)Lookup_Get(MapsLookup, a->type.of);
                if(map == NULL){
                    args[0] = Type_ToStr(m, a->type.of);
                    args[1] = NULL;
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
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
                        void **aa = ((void *)a)+att->range;
                        if(pack){
                            StashItem *item = (StashItem *)Table_Get(tbl, 
                                Util_Wrapped(m, (util)*aa));
                            if(item != NULL){
                                Stash_PackAddr(item->coord.typeOf,
                                    item->coord.idx, (void **)aa);
                                StashCoord *coord = (StashCoord *)aa;
                            }else{
                                args[0] = Util_Wrapped(m, (util)aa);
                                args[1] = map->keys[i];
                                args[2] = NULL;
                                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                                    "Att Unable to find address $ for $ in table,"
                                    " may be external to this MemCh", args);
                            }
                        }else{
                            StashCoord *coord = (StashCoord *)aa;
                            Stash_UnpackAddr(m, coord, mit->input.arr);
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

status Stash_FlushFree(Buff *bf, MemCh *persist){
    DebugStack_Push(persist, persist->type.of);
    status r = READY;
    SourceFunc func = NULL;
    void *a = NULL;
    void *args[5];
    Iter it;

    MemCh *m = bf->m;
    Table *tbl = Table_Make(m);

    MemIter mit;
    MemIter_Init(&mit, persist);
    i32 count = 0;
    while((MemIter_Next(&mit) & END) == 0){
        Abstract *a = (Abstract *)MemIter_Get(&mit);
        if((mit.type.state & MORE) == 0){
            void *ptr = (void *)a;
            if(a->type.of < _TYPE_RANGE_TYPE_END){
                void *orig = ptr;
                ptr += sizeof(RangeType);
            }

            Table_Set(tbl, Util_Wrapped(m, (util)ptr), 
                StashItem_Make(m, mit.slIdx, (void *)ptr, a->type.of));
        }
    }

    Span *pages = Span_Make(m);
    Iter_Reset(&persist->it);
    while((Iter_Next(&persist->it) & END) == 0){
        Abstract *a = (Abstract *)Iter_Get(&persist->it);
        Table_Set(tbl, Util_Wrapped(m, (util)a), 
            StashItem_Make(m, persist->it.idx, (void *)a, a->type.of));
        Span_Add(pages, a);
    }

    MemIter_Init(&mit, persist);
    i16 checksum = Stash_PackMemCh(m, &mit, tbl, NULL);

    StashHeader hdr = {
        .pages = (i16)pages->nvalues,
        .checksum = checksum
    };
    Buff_AddBytes(bf, (byte *)&hdr, sizeof(StashHeader));
    Buff_Flush(bf);

    Iter_Init(&it, pages);
    while((Iter_Next(&it) & END) == 0){
        MemPage *pg = (MemPage *)Iter_Get(&it);
        if((Buff_AddBytes(bf, (byte *)pg, PAGE_SIZE) & SUCCESS) == 0){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error writing page to stream for Stash", NULL);
            r |= ERROR;
            break;
        }
        Buff_Flush(bf);
        r |= MemBook_FreePage(m, (MemPage *)Iter_Get(&it));
    }

    DebugStack_Pop();
    return r;
}

MemCh *Stash_FromStream(Buff *bf){
    DebugStack_Push(bf, bf->type.of);
    status r = READY;

    void *args[5];
    StashHeader hdr = {0, 0};
    i16 count = 0;
    MemCh *persist = NULL; 
    void **pages = NULL;
    MemCh *m = bf->m;
    Str s = {
        .type = {TYPE_STR, ZERO},
        .length = 0,
        .alloc = 0,
        .bytes = NULL,
    };
    while((r & (SUCCESS|ERROR)) == 0 && (bf->type.state & END) == 0){
        if((r & PROCESSING) == 0){
            s.alloc = sizeof(StashHeader);
            s.length = 0;
            s.bytes = (byte *)&hdr;
            if((Buff_GetStr(bf, &s) & SUCCESS) == 0){
                r |= ERROR;
                break;
            }

            r |= PROCESSING;
            pages = (void **)Bytes_Alloc(bf->m, hdr.pages*sizeof(void *), TYPE_POINTER_ARRAY);
        }

        if(count >= hdr.pages){
            r |= SUCCESS;
            break;
        }

        pages[count] = MemBook_GetPage(NULL);
        if(pages[count] == NULL){
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error allocating page", NULL);
            r |= ERROR;
            DebugStack_Pop();
            return NULL;
        }

        s.alloc = PAGE_SIZE;
        s.length = 0;
        s.bytes = (byte *)pages[count];
        r |= (Buff_GetStr(bf, &s) & SUCCESS);
        if((r & SUCCESS) == 0){
            args[0] = I16_Wrapped(ErrStream->m, s.length);
            args[1] = NULL;
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error reading page from stream to Stash length $", args);
            r |= ERROR;
        }

        count++;
    }

    i16 checksum = 0;

    if(r & SUCCESS){
        r &= ~SUCCESS;
        MemIter mit;
        MemIter_InitArr(&mit, pages, hdr.pages-1);
        checksum = Stash_PackMemCh(m, &mit, NULL, &persist);
    }

    if(checksum != hdr.checksum){
        args[0] = I16_Wrapped(bf->m, hdr.checksum);
        args[1] = I16_Wrapped(bf->m, checksum);
        args[2] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error checksum of number of changes does not match for resurecting persisting"             " expected $, have $",
            args);
        r |= ERROR;
    }else{
        r |= SUCCESS;
    }

    if((r & (SUCCESS|ERROR)) == SUCCESS){
        DebugStack_Pop();
        return persist;
    }
    
    DebugStack_Pop();
    return persist;
}

status Stash_Init(MemCh *m){
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
