#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;
static Str **nodeLabels = NULL;
static Str **messLabels = NULL;
static Str **stepLabels = NULL;
static Str **taskLabels = NULL;
static Str **queueLabels = NULL;

static status indentStream(Buff *bf, i32 indent){
    while(--indent >= 0){
        Buff_AddBytes(bf, (byte *)"  ", 2);
    }
    return SUCCESS;
}

static status Node_ToBinSeg(BinSegCtx *ctx, void *a, i16 id){
    Node *nd = (Node *)as(a, TYPE_NODE);
    MemCh *m = ctx->bf->m;
    status r = READY;

    i32 childCount = 1;
    if(nd->typeOfChild == TYPE_SPAN){
        childCount = ((Span *)nd->child)->nvalues;
    }

    Str *content;
    Str *entry;
    BinSegHeader *hdr = BinSegHeader_Make(ctx,
        childCount,
        BINSEG_TYPE_NODE,
        id,
        &content,
        &entry);

    if(ctx->type.state & BINSEG_REVERSED){
        r |= BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    i16 *ip = (i16 *)content->bytes;
    if(nd->value == NULL){
        *ip = 0;
    }else{
        *ip = ctx->func(ctx, NULL);
        r |= BinSegCtx_Send(ctx, nd->value, *ip);
    }

    ip++;
    if(nd->atts == NULL){
        *ip = 0;
    }else{
        *ip = ctx->func(ctx, NULL);
        r |= BinSegCtx_Send(ctx, nd->atts, *ip);
    }
    ip++;

    if(nd->typeOfChild == TYPE_SPAN){
        Span *p = (Span *)nd->child;
        for(i32 i = 0; i < p->nvalues; i++){
            *ip = ctx->func(ctx, NULL);
            ip++;
        }
    
        i16 *ip = (i16 *)content->bytes;
        ip += 2;

        Iter it;
        Iter_Init(&it, p);
        while((Iter_Next(&it) & END) == 0){
            r |= BinSegCtx_Send(ctx, Iter_Get(&it), *ip);
            ip++;
        };
    }else{
        *ip = ctx->func(ctx, NULL);
        r |= BinSegCtx_Send(ctx, nd->child, *ip);
    }

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    return r;
}

static status Mess_ToBinSeg(BinSegCtx *ctx, void *a, i16 id){
    Mess *mess = (Mess *)as(a, TYPE_MESS);
    return Node_ToBinSeg(ctx, a, id);
}

static status CompResult_Print(Buff *bf, void *a, cls type, word flags){
    CompResult *cr = (CompResult*)as(a, TYPE_COMPRESULT);
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
   QueueCrit *crit = (QueueCrit *)as(a, TYPE_QUEUE_CRIT);
   void *args[] = {
        NULL
   };
   return Fmt(bf, "QueueCrit<>", args);
}

static status Queue_Print(Buff *bf, void *a, cls type, word flags){
    Queue *q = (Queue *)as(a, TYPE_QUEUE);
    status r = READY;
    void *args[6];
    args[0] = Type_StateVec(bf->m, q->type.of, q->type.state);
    args[1] = NULL;
    r |= Fmt(bf, "Queue<@ ", args);
    r |= Bits_Print(bf, (byte *)&q->go, sizeof(word), ZERO);
    if(flags & DEBUG){
        args[0] = &q->it;
        args[1] = q->handlers;
        args[2] = &q->availableIt;
        args[3] = NULL;
        r |= Fmt(bf, " @ criteria:@ available:p>", args);
    }else{
        args[0] = I32_Wrapped(bf->m, q->it.p->nvalues);
        args[1] = NULL;
        r |= Fmt(bf, " @nvalues>", args);
    }

    return r;
}

static status Comp_Print(Buff *bf, void *a, cls type, word flags){
    Comp *comp = (Comp*)as(a, TYPE_COMP);
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

static status Node_Print(Buff *bf, void *a, cls type, word flags){
    Node *nd = (Node*)as(a, TYPE_NODE);
    void *args[9];
    args[0] = Type_StateVec(bf->m, nd->type.of, nd->type.state);
    args[1] = Type_ToStr(bf->m, nd->typeOfChild);
    args[2] = Type_ToStr(bf->m, nd->captureKey);
    args[3] = (nd->parent != NULL ?
                  Type_ToStr(bf->m, nd->parent->captureKey) : NULL);
    args[4] = nd->atts;
    args[5] = Type_ToStr(bf->m,
                 (nd->value != NULL ? nd->value->type.of : _TYPE_ZERO));
    args[6] = nd->value;
    args[7] = Type_ToStr(bf->m,
                 (nd->child != NULL ? nd->child->type.of : _TYPE_ZERO));
    args[8] =  NULL;
    if(flags & DEBUG){
        return Fmt(bf, "N<$/$ captureKey($) parent(@) atts:& valueTypeOf:$/@ childTypeOf:$>", args);
    }else{
        args[4] = args[5];
        args[5] = nd->value;
        args[6] = NULL;
        return Fmt(bf, "N<$/$ captureKey($) parent(@) value:$/@>", args);
    }
}

static status Relation_Print(Buff *bf, void *a, cls type, word flags){
    status r = READY;
    Relation *rel = (Relation*)as(a, TYPE_RELATION);
    void *args[] = {
        I16_Wrapped(bf->m, rel->stride),
        I32_Wrapped(bf->m, Relation_RowCount(rel)),
        (rel->headers != NULL ?
            Ptr_Wrapped(bf->m, rel->headers, TYPE_ARRAY): NULL),
        NULL
    };
     Fmt(bf, "Rel<$x$ @ [", args);
    if(rel->it.p->max_idx >= 0){
        Buff_AddBytes(bf, (byte *)"\n", 1);
    }

    while((Relation_Next(rel) & END) == 0){
        if(rel->type.state & RELATION_ROW_START){
            Buff_AddBytes(bf, (byte *)"  ", 2);
        }
        ToS(bf, rel->it.value, 0, flags);
        if(rel->type.state & RELATION_ROW_END){
            Buff_AddBytes(bf, (byte *)",\n", 2);
        }else{
            Buff_AddBytes(bf, (byte *)",", 1);
        }
    }

    return Buff_AddBytes(bf, (byte *)"]>", 2);
}

static status MessClimber_PrintItems(Buff *bf, MessClimber *climber, word flags){
    i32 nested = ++climber->nested;
    if(climber->current != NULL){
        void *current = climber->current;
        Buff_AddBytes(bf, (byte *)"\n", 1);
        while(nested--){
            Buff_AddBytes(bf, (byte *)"  ", 2);
        }
        if(current->type.of == TYPE_NODE){
            Node *nd = (Node *)climber->current;
            ToS(bf, nd, 0, flags);
            if(nd->typeOfChild == TYPE_SPAN){
                Buff_AddBytes(bf, (byte *)"[", 1);
                Iter it;
                Iter_Init(&it, (Span *)nd->child);
                while((Iter_Next(&it) & END) == 0){
                    climber->current = it.value;
                    MessClimber_PrintItems(bf, climber, flags);
                }
                Buff_AddBytes(bf, (byte *)"]", 1);
            }else if(nd->typeOfChild == TYPE_NODE){
                climber->current = nd->child;
                Buff_AddBytes(bf, (byte *)"<", 2);
                MessClimber_PrintItems(bf, climber, flags);
                Buff_AddBytes(bf, (byte *)">", 1);
            }else{
                climber->current = nd->child;
                MessClimber_PrintItems(bf, climber, flags);
            }
        }else if(current->type.of == TYPE_SPAN){
            Iter it;
            Iter_Init(&it, (Span *)current);
            while((Iter_Next(&it) & END) == 0){
                climber->current = it.value;
                MessClimber_PrintItems(bf, climber, flags);
            }
        }else{
            ToS(bf, current, 0, MORE|DEBUG);
        }
        climber->current = current;
    }

    climber->nested--;
    return SUCCESS;
}

static status Mess_Print(Buff *bf, void *a, cls type, word flags){
    Mess *ms = (Mess *)as(a, TYPE_MESS);
    if((flags & (DEBUG|MORE)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }else{
        void *args[] = {
            Type_StateVec(bf->m, ms->type.of, ms->type.state),
            NULL
        };
        Fmt(bf, "Mess<$", args);
        if(flags & DEBUG){
            void *args[] = {
                ms->current,
                ms->currentValue,
                NULL
            };
            Fmt(bf, " current:$ value:@ [", args);
        }else{
            Buff_AddBytes(bf, (byte *)" [", 2);
        }
        MessClimber climber = {
            .type = {TYPE_MESS_CLIMBER, 0},
            .nested = 1,
            .mess = ms,
            .current = ms->root,
        };
        MessClimber_PrintItems(bf, &climber, flags|DEBUG);
        return Buff_AddBytes(bf, (byte *)"\n]>", 3);
    }
}

static status Step_Print(Buff *bf, void *a, cls type, word flags){
    Step *st = (Step *)as(a, TYPE_STEP);
    void *args[5];
    args[0] = Type_StateVec(bf->m, st->type.of, st->type.state);
    args[1] = Util_Wrapped(bf->m, (util)st->func);
    args[4] = NULL;
    if(flags & DEBUG){
        args[2] = st->arg;
        args[3] = st->source;
        return Fmt(bf, "Step<$ ^D.$^d.func arg:@ source:@>", args);
    }else{
        args[2] = Type_ToStr(bf->m,
            st->arg != NULL ? st->arg->type.of : ZERO);
        args[3] = Type_ToStr(bf->m,
            st->source != NULL ? st->source->type.of : ZERO);
        return Fmt(bf, "Step<$ ^D.$^d.func arg:$ source:$>", args);
    }
}

static status Task_Print(Buff *bf, void *a, cls type, word flags){
    Task *tsk = (Task *)as(a, TYPE_TASK);
    void *args[6];
    args[0] = Type_StateVec(bf->m, tsk->type.of, tsk->type.state);
    args[1] = I32_Wrapped(bf->m, tsk->chainIt.idx);
    args[2] = I32_Wrapped(bf->m, tsk->chainIt.p->max_idx);
    args[3] = Iter_Current(&tsk->chainIt);
    args[5] = NULL;
    if(flags & DEBUG){
        args[4] = tsk->data;
        return Fmt(bf, "Task<@ $ of $ \\@& data:@>", args);
    }else{
        args[1] = tsk->data;
        args[2] = NULL;
        return Fmt(bf, "Task<@ @>", args);
    }
}

static status Frame_Print(Buff *bf, void *a, cls type, word flags){
    Frame *fm = (Frame *)as(a, TYPE_FRAME);
    void *args[] = {
        I32_Wrapped(bf->m, fm->originIdx),
        fm->originKey,
        (fm->value != NULL ? Type_ToStr(bf->m, fm->value->type.of): NULL),
        I32_Wrapped(bf->m, fm->it.idx),
        Iter_Current(&fm->it),
        NULL,
    };
    return Fmt(bf, "Frame<from:@/@ @[@]>", args);
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
    if(nodeLabels == NULL){
        nodeLabels = (Str **)Arr_Make(m, 17);
        nodeLabels[9] = Str_CstrRef(m, "FLAG_CHILDREN");
        nodeLabels[10] = Str_CstrRef(m, "FLAG_CHILD");
        Lookup_Add(m, lk, TYPE_NODE, (void *)nodeLabels);
        r |= SUCCESS;
    }

    if(stepLabels == NULL){
        stepLabels = (Str **)Arr_Make(m, 17);
        stepLabels[9] = Str_CstrRef(m, "IO_IN");
        stepLabels[10] = Str_CstrRef(m, "IO_OUT");
        stepLabels[11] = Str_CstrRef(m, "LOOP");
        Lookup_Add(m, lk, TYPE_STEP, (void *)stepLabels);
        r |= SUCCESS;
    }

    if(taskLabels == NULL){
        taskLabels = (Str **)Arr_Make(m, 17);
        taskLabels[9] = Str_CstrRef(m, "UPDATE_CRIT");
        taskLabels[10] = Str_CstrRef(m, "QUEUE");
        Lookup_Add(m, lk, TYPE_TASK, (void *)taskLabels);
        r |= SUCCESS;
    }

    if(queueLabels == NULL){
        queueLabels = (Str **)Arr_Make(m, 17);
        queueLabels[9] = Str_CstrRef(m, "SINGLE_IDX");
        queueLabels[16] = Str_CstrRef(m, "REVERSE");
        Lookup_Add(m, lk, TYPE_QUEUE, (void *)queueLabels);
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Navigate_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_MESS, (void *)Mess_Print);
    r |= Lookup_Add(m, lk, TYPE_NODE, (void *)Node_Print);
    r |= Lookup_Add(m, lk, TYPE_RELATION, (void *)Relation_Print);
    r |= Lookup_Add(m, lk, TYPE_COMP, (void *)Comp_Print);
    r |= Lookup_Add(m, lk, TYPE_COMPRESULT, (void *)CompResult_Print);
    r |= Lookup_Add(m, lk, TYPE_FRAME, (void *)Frame_Print);
    r |= Lookup_Add(m, lk, TYPE_TASK, (void *)Task_Print);
    r |= Lookup_Add(m, lk, TYPE_STEP, (void *)Step_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE, (void *)Queue_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE_CRIT, (void *)QueueCrit_Print);
    return r;
}

status Navigate_BinSegInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_NODE, (void *)Node_ToBinSeg);
    r |= Lookup_Add(m, lk, TYPE_MESS, (void *)Mess_ToBinSeg);
    return r;
}

status Navigate_ClsInit(MemCh *m){
    status r = READY;
    r |= Navigate_ToSInit(m, ToStreamLookup);
    r |= Navigate_InitLabels(m, ToSFlagLookup);
    r |= Navigate_BinSegInit(m, BinSegLookup);
    return r;
}
