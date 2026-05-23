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
    Debug_Push(m, mit);
    boolean pack = (mit->type.state & MEM_ITER_STREAM) == 0;
    void *args[5];
    i16 checksum = 0;
    i32 count = 0;
    while((MemIter_Next(mit) & END) == 0){
        MemIdent *mid = (MemIdent *)MemIter_Get(mit);

        if((mit->type.state & MORE) == 0){

            void *ar[] = {mid, Util_Wrapped(m, (util)(mit->current.ptr - (void *)mit->page)), NULL};
            if(pack){
                Out("^c.Mid to Pack \\@$ @^0\n", ar);
            }else{
                Out("^c.Mid to Unpack \\@$ @^0\n", ar);
            }

            if(mid->rtype.of == TYPE_POINTER_ARRAY){
                BytesLit *bt = (BytesLit*)mid->ptr;
                i16 total = bt->type.range / sizeof(void *);
                void **dptr = (void **)Bytes_Ptr(bt);
                for(i16 i = 0; i < total; i++){
                    void *ptr = *dptr;
                    if(ptr == NULL){
                        break;
                    }
                    if(pack){
                        MemIdent *item = (MemIdent *)Table_Get(tbl, 
                            Util_Wrapped(m, (util)ptr));

                        if(item != NULL && item->content != NULL){
                            Stash_PackAddr(item->rtype.of,
                                item->slIdx, (void **)item->content);

                            StashCoord *coord = (StashCoord *)dptr;
                        }else{
                            args[0] = Util_Wrapped(m, (util)ptr);
                            args[1] = item;
                            args[2] = NULL;
                            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                                "Packing PTR ARRAY Unable to find address @ or content in table, have @, "
                                " may be external to this MemCh", args);
                        }
                    }else{
                        StashCoord *coord = (StashCoord *)dptr;
                        Stash_UnpackAddr(m, coord, mit->input.arr);
                    }
                    checksum++;
                    dptr++;
                }
            }else if(mid->rtype.of != TYPE_MEMSLAB &&
                    mid->rtype.of > _TYPE_ABSTRACT_BEGIN){
                Map *map = (Map *)Lookup_Get(MapsLookup, mid->rtype.of);
                if(map == NULL){
                    args[0] = Type_ToStr(m, mid->rtype.of);
                    args[1] = NULL;
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Map not found for type $, needed for mem persist", args);
                    Return(m, ERROR);
                }
                for(i16 i = 1; i <= map->type.range; i++){
                    RangeType *att = map->atts+i;
                    if(att->of > _TYPE_RANGE_TYPE_START){
                        void **aa = ((void *)mid->content)+att->range;
                        if(pack){
                            MemIdent *item = (MemIdent *)Table_Get(tbl, 
                                Util_Wrapped(m, (util)*aa));
                            if(item != NULL && item->content != NULL){
                                Stash_PackAddr(item->rtype.of,
                                    item->slIdx, (void **)aa);
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
                if(!pack && mid->rtype.of == TYPE_MEMCTX){
                    *persist = (MemCh *)mid->content;
                }
            }else{
                Str *s = Str_Ref(m, (byte *)mid->ptr, sizeof(Type), sizeof(Type), DEBUG);
                void *ar[] = {mid, s, NULL};
                Out("^c Not changing @/@^0\n", ar);
            }
        }
    }

    Return(m, checksum);
}

status Stash_FlushFree(Buff *bf, MemCh *persist){
    MemCh *m = bf->m;
    Debug_Push(m, persist);

    status r = READY;
    SourceFunc func = NULL;
    void *a = NULL;
    void *args[5];

    Table *tbl = MemIter_GetTable(m, persist);

    Iter it;
    Iter pagesIt;
    Iter_Init(&pagesIt, Span_Make(m));
    Iter_Init(&it, persist->it.p);
    while((Iter_Next(&it) & END) == 0){
        MemPage *pg = (MemPage *)Iter_Get(&it);
        Str *s = Str_Ref(m, (byte *)pg, MEM_SLAB_SIZE, MEM_SLAB_SIZE, DEBUG);
        void *ar[] = {
            pg,
            s,
            NULL
        };
        printf("%p", pg);
        fflush(stdout);
        Out("^p.Adding Page @ - @^0\n", ar);
        Iter_Add(&pagesIt, pg);
    }

    i32 count = 0;

    MemIter mit;
    MemIter_Init(m, &mit, persist);
    i16 checksum = Stash_PackMemCh(m, &mit, tbl, NULL);

    StashHeader hdr = {
        .pages = (i16)pagesIt.p->nvalues,
        .checksum = checksum
    };
    Buff_AddBytes(bf, (byte *)&hdr, sizeof(StashHeader));
    Buff_Flush(bf);

    Iter_Reset(&pagesIt);
    while((Iter_Next(&pagesIt) & END) == 0){
        MemPage *pg = (MemPage *)Iter_Get(&pagesIt);

        printf("%p", pg);
        fflush(stdout);
        Str *s = Str_Ref(m, (byte *)pg, PAGE_SIZE, PAGE_SIZE, DEBUG);
        void *ar[] = {I32_Wrapped(m, pagesIt.idx), s, NULL};
        Out("^c.Persisting Page $ @^0\n", ar);


        if((Buff_AddBytes(bf, (byte *)pg, PAGE_SIZE) & SUCCESS) == 0){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error writing page to stream for Stash", NULL);
            r |= ERROR;
            break;
        }

        Buff_Flush(bf);
        r |= MemBook_FreePage(persist, (MemPage *)Iter_Get(&pagesIt));
    }

    Return(m, r);
}

MemCh *Stash_FromStream(Buff *bf){
    Debug_Push(bf->m, bf);

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

            Return(m, NULL);
        }

        s.alloc = PAGE_SIZE;
        s.length = 0;
        s.bytes = (byte *)pages[count];
        r |= (Buff_GetStr(bf, &s) & SUCCESS);

        s.type.state |= DEBUG;
        void *ar[] = {&s, NULL};
        Out("^c.Page @^0\n", ar);

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
        MemIter_InitArr(bf->m, &mit, pages, hdr.pages-1);
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
        Return(m, persist);
    }

    Return(m, persist);
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
