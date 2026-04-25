#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;
static Str **stepLabels = NULL;
static Str **taskLabels = NULL;
static Str **queueLabels = NULL;
static Str **iterUpperLabels = NULL;

static status indentStream(Buff *bf, i32 indent){
    while(--indent >= 0){
        Buff_AddBytes(bf, (byte *)"  ", 2);
    }
    return SUCCESS;
}

static status CompResult_Print(Buff *bf, void *a, cls type, word flags){
    CompResult *cr = (CompResult*)Ifc(bf->m, a, TYPE_COMPRESULT);
    void *args[] = {
        Type_StateVec(bf->m, cr->type.of, cr->type.state),
        cr->a,
        cr->b,
        NULL,
    };
    if(flags & DEBUG){
        return Fmt(bf, "(^D$^d.\n    ^D.A^d.&,\n    ^D.B^d.&)", args);
    }else{
        return Fmt(bf, "(^D$^d.\n    $,\n    $)", args);
    }
}

static status QueueCrit_Print(Buff *bf, void *a, cls type, word flags){
    QueueCrit *crit = (QueueCrit *)Ifc(bf->m, a, TYPE_QUEUE_CRIT);
    void *args[2];
    if(flags & MORE){
        Table *tbl = Table_Make(bf->m);
        Iter it;   
        Iter_Init(&it, crit->data);
        while((Iter_Next(&it) & END) == 0){
            util *slab = (util *)Iter_Get(&it);
            for(i32 i = 0; i < CRIT_SLAB_STRIDE; i++){
               if(slab[i] != -1 && slab[i] != 0){
                   Single *key = I64_Wrapped(bf->m, it.idx *CRIT_SLAB_STRIDE + i);
                   Single *value = NULL;
                   if(crit->type.state & QUEUE_CRIT_PFD){
                        struct pollfd *pfd = (struct pollfd *)slab+i;
                        value = I64_Wrapped(bf->m, pfd->fd);
                   }else{
                        value = I64_Wrapped(bf->m, slab[i]);
                   }
                   Table_Set(tbl, key, value);
                }
            }
        }
        
        args[0] = Table_Ordered(bf->m, tbl);
        args[1] = NULL;
        return Fmt(bf, "QueueCrit<@>", args);
    }else{
        args[0] = NULL;
        return Fmt(bf, "QueueCrit<>", args);
    }
}

static status Queue_Print(Buff *bf, void *a, cls type, word flags){
    Queue *q = (Queue *)Ifc(bf->m, a, TYPE_QUEUE);
    status r = READY;
    void *args[7];
    args[0] = Type_StateVec(bf->m, q->type.of, q->type.state);
    args[1] = NULL;
    r |= Fmt(bf, "Queue<@ ", args);
    r |= Bits_Print(bf, (byte *)&q->go, sizeof(util), ZERO);
    if(flags & DEBUG){
        args[0] = I32_Wrapped(bf->m, q->slabIdx*CRIT_SLAB_STRIDE);
        args[1] = I32_Wrapped(bf->m, ((q->slabIdx+1)*CRIT_SLAB_STRIDE)-1);
        args[2] = &q->it;
        args[3] = q->handlers;
        args[4] = q->it.p;
        args[5] = &q->availableIt;
        args[6] = NULL;
        r |= Fmt(bf, " $to$ it:@ criteria:@ items:& available:@>", args);
    }else{
        args[0] = I32_Wrapped(bf->m, q->it.p->nvalues);
        args[1] = NULL;
        r |= Fmt(bf, " @nvalues>", args);
    }

    return r;
}

static status Comp_Print(Buff *bf, void *a, cls type, word flags){
    Comp *comp = (Comp*)Ifc(bf->m, a, TYPE_COMP);
    void *args[] = {
        Type_StateVec(bf->m, comp->type.of, comp->type.state),
        &comp->it,
        NULL
    };
    Fmt(bf, "Comp<$/It(@)\n", args);
    Iter it;
    Iter_Init(&it, comp->it.p);
    while((Iter_Prev(&it) & END) == 0){
        Buff_AddBytes(bf, (byte *)"  ", 2);
        ToS(bf, it.value, 0, DEBUG|flags);
        Buff_AddBytes(bf, (byte *)"\n", 1);
    }
    return Buff_AddBytes(bf, (byte *)">", 1);
}

status Navigate_InitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    /*
    if(messLabels == NULL){
        messLabels = (Str **)Arr_Make(m, 17);
        Lookup_Add(m, lk, TYPE_PATMATCH, (void *)messLabels);
        r |= SUCCESS;
    }
    */

    if(queueLabels == NULL){
        queueLabels = (Str **)Arr_Make(m, 17);
        queueLabels[9] = Str_CstrRef(m, "SINGLE_IDX");
        queueLabels[16] = Str_CstrRef(m, "REVERSE");
        Lookup_Add(m, lk, TYPE_QUEUE, (void *)queueLabels);
        r |= SUCCESS;
    }

    if(iterUpperLabels == NULL){
        iterUpperLabels = (Str **)Arr_Make(m, 17);
        iterUpperLabels[9] = Str_CstrRef(m, "REQUIRED");
        iterUpperLabels[10] = Str_CstrRef(m, "FOCUS");
        iterUpperLabels[11] = Str_CstrRef(m, "ACTION");
        iterUpperLabels[12] = Str_CstrRef(m, "FINISH");
        iterUpperLabels[13] = Str_CstrRef(m, "SKIP");
        iterUpperLabels[14] = Str_CstrRef(m, "SIBLING");
        iterUpperLabels[15] = Str_CstrRef(m, "LEAF");
        iterUpperLabels[16] = Str_CstrRef(m, "ENCLOSE");
        Lookup_Add(m, lk, TYPE_ITER_UPPER, (void *)iterUpperLabels);
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Navigate_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Navigate_InitLabels(m, ToSFlagLookup);
    r |= Lookup_Add(m, lk, TYPE_COMP, (void *)Comp_Print);
    r |= Lookup_Add(m, lk, TYPE_COMPRESULT, (void *)CompResult_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE, (void *)Queue_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE_CRIT, (void *)QueueCrit_Print);
    return r;
}
