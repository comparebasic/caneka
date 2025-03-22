
static void Queue_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Queue *q = as(a, TYPE_QUEUE);
    printf("\x1b[%dm%sQ<%s\x1b[0m", color, msg, State_ToChars(q->type.state));
    if(extended){
        Debug_Print((void *)q->span, 0, "", color, TRUE);
        printf("\n");
        Debug_Print((void *)&q->current, 0, "current=", color, FALSE);
        printf("\n");
        Debug_Print((void *)&q->available, 0, "available=", color, FALSE);
        printf("\n");
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm values:%d current:%d available:%d>\x1b[0m", color, q->span->nvalues, q->current.idx, q->available.idx);
    }
}



void NestedD_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    NestedD *nd = (NestedD *)as(a, TYPE_NESTEDD);
    printf("\x1b[%dmND<%s: ", color, State_ToChars(nd->type.state));
    Debug_Print((void *)nd->current_tbl, 0, "", color, extended);
    Iter it;
    Iter_InitReverse(&it, nd->stack);
    printf("\x1b[%dm stack/%d=(", color, nd->stack->nvalues);
    while((Iter_Next(&it) & END) == 0){
        NestedState *ns = (NestedState *)Iter_Get(&it);
        printf("\x1b[%dm%s -> ", color, NestedD_opToChars(ns->flags));
        Debug_Print((void *) ns->t, 0, "", color, FALSE);
        if((it.type.state & FLAG_ITER_LAST) == 0){
            printf("\x1b[%dm,", color);
        }
    }
    printf("\x1b[%dm)>\x1b[0m", color);
}



void TableChain_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    TableChain *tch = (TableChain *)as(a, TYPE_TABLE_CHAIN);
    printf("\x1b[%dmTChain<", color);
    if(extended){
        while(tch != NULL){
            printf("\n    ");
            Debug_Print((Abstract *)tch->tbl, 0, "tbl: ", color, extended);
            tch = tch->next;
        }
    }
    if(extended){
        while(tch != NULL){
            printf("\x1b[%dmtbl(%d)", color, tch->tbl != NULL ? tch->tbl->nvalues : 0);
            if(tch != NULL){
                printf("\x1b[%dm, ", color);
            }
        }
    }
    printf("\x1b[%dm>\x1b[0m", color);

}

void OrdTable_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    OrdTable *otbl = (OrdTable *)as(a, TYPE_ORDERED_TABLE);
    Debug_Print((void *)otbl->order, 0, msg, color,extended); 
}

void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Span *p = (Span *)asIfc(a, TYPE_SPAN);
    printf("\x1b[0;%dm%sP<%u items in dims:%u ", color, msg,
        p->nvalues, p->dims);
    if(extended){
        if(p->metrics.selected >= 0){
            printf("sel:%d ", p->metrics.selected);
        }
        if(p->type.of == TYPE_QUEUE_SPAN){
            printf("%s ", QueueFlags_ToChars(p->type.state));
        }
        printf("\n");
        Slab_Print(p->root, color, p->dims, 0, 1, TRUE, p->dims, 0);
    }
    printf(">\x1b[0m");
}

static void mess_PrintRecurse(Mess *ms, char *msg, int color, boolean extended, int indent){
    /*
    indent_Print(indent);
    printf("\x1b[%dm%sM<%s value=\x1b[0m", color, msg, Class_ToString(ms->type.of));
    printf(" ");
    if(ms->atts != NULL){
        printf("\x1b[%dmatts=[", color);
    }
    Iter it;
    Iter_Init(&it, ms->atts);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            Debug_Print((void *)h, 0, "", color, extended);
        }
    }
    printf("\x1b[%dm, ", color);
    if(ms->atts != NULL){
        printf("\x1b[%dm]", color);
    }
    Debug_Print(ms->body, 0, "body=", color, TRUE);
    Mess *child = ms->firstChild;
    if(child != NULL){
        printf("\x1b[%dm [", color);
        printf("\n");
    }
    while(child != NULL){
        mess_PrintRecurse(child, "", color, extended, indent+1);
        child = child->next;
    }
    if(ms->next != NULL){
        printf("\x1b[%dmnext=%p", color, ms->next);
    }
    if(ms->firstChild != NULL){
        indent_Print(indent);
        printf("\x1b[%dm]", color);
        printf(">\x1b[0m");
    }
    printf("\n");
    */
}

static void Mess_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Mess *ms = (Mess *)a;
    mess_PrintRecurse(ms, msg, color, extended, 0);
}

status NavigateDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_QUEUE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE, (void *)Queue_Print);
    r |= Lookup_Add(m, lk, TYPE_MESS, (void *)Mess_Print);
    r |= Lookup_Add(m, lk, TYPE_NESTEDD, (void *)NestedD_Print);
    r |= Lookup_Add(m, lk, TYPE_ORDERED_TABLE, (void *) OrdTable_Print);

    return r;
}

