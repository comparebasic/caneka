#include <basictypes_external.h> 
#include <caneka/caneka.h> 

String *Slab_ToString(MemCtx *m, Slab *sl, Span *sp){
    String *ret = String_New(m);
    String_Extend(ret, String_FormatV(m, "Slab<", "%d", &(sl->id), ">", NULL));
    return ret;
}

String *Slab_ToDebug(MemCtx *m, Slab *sl, Span *sp){
    lifecycle_t prevCycle = m->destCycle;
    if(m->destCycle == 0){
        m->destCycle = MEM_TEMP;
    }

    String *build = String_New(m);
    int start = sl->local_idx * sl->increment + sl->offset;
    int end = ((sl->local_idx+1) * sl->increment)-1 +sl->offset;
    String_Extend(build, String_FormatV(m, 
        "Slab(", "%d", &(sl->id), "/", "%d", &(sl->level), "<", "from:", "%d", &start,"-", "%d", &end, "/", "%d", &(sl->increment), NULL));
    String_Extend(build, Nstr(m, " ["));
    if(sl->level > 1){
          for(int i = 0; i < sl->stride; i++){
                Typed *t = sl->items[i];
                if(t != NULL){
                    String_Extend(build, String_FormatV(m, Typed_TypeAbbrevToCStr(t->type), "(", "%d", &(t->id), ")", NULL));
                }
                if(i+1 < sl->stride){
                    String_Extend(build, Nstr(m, ","));
                }
          }
    }else{
        if(sp == NULL){
            String_Extend(build, Nstr(m, "items"));
        }else{
           if(HasFlag(sp, FLAG_RAW)){
               int slotSize = sp->slotSize;
               for(int i = 0; i < sl->stride; i += slotSize){
                    void *ptr = ((void *)sl->items) + (i*sizeof(void *));
                    if((*((Typed **)ptr)) != NULL){
                        Typed *wr = Typed_WrapRaw(m, ptr, sp->itemType, sp->itemSize);
                        String_Extend(build, Typed_ToString(m, wr));
                    }
                    if(i+slotSize < sl->stride){
                        String_Extend(build, Nstr(m, ","));
                    }
               }
           }else{
              for(int i = 0; i < sl->stride; i++){
                    Typed *t = sl->items[i];
                    int idx = start+i;
                    if(t != NULL){
                        String_Extend(build, String_FormatV(m, "%d", &idx, "=", "%S", Typed_ToString(m, t),  "/", Typed_TypeAbbrevToCStr(t->type), NULL));
                    }
                    if(i+1 < sl->stride){
                        String_Extend(build, Nstr(m, ","));
                    }
              }
           }
        }
    }
    String_Extend(build, Nstr(m, "]>"));

    m->destCycle = prevCycle;
    SetFlag(build, STRING_REALIGN);
    return  String_Clone(m, build);
}

String *Span_PrintSlabsByLevel(MemCtx *m, Slab *sl, Span *sp, int level){
    if(sl == NULL){
        return Nstr(m, "NULL");
    }

    lifecycle_t prevCycle = m->destCycle;
    if(m->destCycle == 0){
        m->destCycle = MEM_TEMP;
    }

    String *indent_s = String_New(m);
    int idt = sp->ndims - sl->level;
    while(idt--){
        String_Extend(indent_s, Nstr(m, "    "));
    }

    String *build = String_New(m);
    if(sl->level == level){
        String_Extend(build, String_Clone(m, indent_s));
        String_Extend(build, Slab_ToDebug(m, sl, sp));
        String_Extend(build, Nstr(m, "\n"));
    }

    if(level >= sl->level){
        m->destCycle = prevCycle;
        SetFlag(build, STRING_REALIGN);
        return  String_Clone(m, build);
    }

    for(int i = 0; i < sl->stride; i++){
          Typed *t = sl->items[i];
          if(t != NULL && t->type == TYPE_SLAB){
               String_Extend(build, Span_PrintSlabsByLevel(m, (Slab *)t, sp, level));
          }
    }

    m->destCycle = prevCycle;
    SetFlag(build, STRING_REALIGN);
    return  String_Clone(m, build);
}


Slab *Slab_Clone(MemCtx *m, Slab *sl){
    Slab *sl2 = Memcpy(m, sl, sizeof(Slab));
    if(sl->level == 1 && HasFlag(sl, FLAG_RAW)){
        sl2->items = Memcpy(m, sl->items, sizeof(Typed **) * sl->stride);
    }else{
        sl2->items = Alloc(m, sizeof(Typed **) * sl->stride);
        for(int i = 0; i < sl->stride; i++){
            if(sl->items[i] != NULL){
                sl2->items[i] = Typed_Clone(m, sl->items[i]);
            }
        }
    }
    return sl2;
}

lifecycle_t Slab_Resize(MemCtx *m, Slab *sl, int idx){
    util_t size = sizeof(Typed *) * (idx+1);
    if(size > sl->nallocated){
        int nalloc = 2;
        if(sl->nallocated >= 2){
            nalloc = sl->nallocated; 
        }

        while(nalloc < idx+1){
            nalloc *= 2;
        }

        if(nalloc > SPAN_DIM_SIZE){
            printf("Error: Slab asked to extend beyond standard dim size\n");
            return ERROR;
        }

        size = sizeof(Typed *) * nalloc;
        if(sl->nallocated == 0){
            sl->items = Alloc(m, size);
        }else{ 
            sl->items = Realloc(m, sl->items, sizeof(Typed *) *sl->nallocated, size);
        }

        sl->nallocated = nalloc;
        return SUCCESS;
    }

    return NOOP;
}

static int slab_id = 0;
Slab* Slab_Alloc(MemCtx* m, int size, flags_t flags){
    Slab *sl = Alloc(m, sizeof(Slab));
    sl->type = TYPE_SLAB;
    sl->flags |= flags;

    sl->nallocated = size;
    sl->items = Alloc(m, sizeof(Typed *) * sl->nallocated);
    sl->id = ++slab_id;

    return sl;
}

