#include <external.h>
#include <caneka.h>

static Str **nodeLabels = NULL;
static Str **messLabels = NULL;

static i64 indentStream(Stream *sm, i32 indent){
    i64 total = 0;
    while(--indent >= 0){
        total += Stream_Bytes(sm, (byte *)"  ", 2);
    }
    return total;
}

static i64 CompResult_Print(Stream *sm, Abstract *a, cls type, word flags){
    CompResult *cr = (CompResult*)as(a, TYPE_COMPRESULT);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)cr, ToS_FlagLabels),
        cr->a,
        cr->b,
        NULL,
    };
    if(flags & DEBUG){
        return Fmt(sm, "(^D$^d.\n    ^D.A^d.&,\n    ^D.B^d.&)", args);
    }else{
        return Fmt(sm, "(^D$^d.\n    $,\n    $)", args);
    }
}

static i64 Comp_Print(Stream *sm, Abstract *a, cls type, word flags){
    Comp *comp = (Comp*)as(a, TYPE_COMP);
    i64 total = 0;
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)comp, ToS_FlagLabels),
        (Abstract *)&comp->it,
        NULL
    };
    total += Fmt(sm, "Comp<$/It(@)\n", args);
    Iter it;
    Iter_Init(&it, comp->it.p);
    while((Iter_Prev(&it) & END) == 0){
        total += Stream_Bytes(sm, (byte *)"  ", 2);
        total += ToS(sm, it.value, 0, DEBUG|flags);
        total += Stream_Bytes(sm, (byte *)"\n", 1);
    }
    total += Stream_Bytes(sm, (byte *)">", 1);
    return total; 
}

static i64 TemplJump_Print(Stream *sm, Abstract *a, cls type, word flags){
    status r = READY;

    TemplJump *jump = (TemplJump *)as(a, TYPE_TEMPL_JUMP);
    Abstract *args[] = {
        (Abstract *)Type_ToStr(sm->m, jump->jumpType.of),
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)jump, ToS_FlagLabels),
        (Abstract *)I32_Wrapped(sm->m, jump->idx),
        (Abstract *)I32_Wrapped(sm->m, jump->destIdx),
        NULL
    };
    return Fmt(sm, "TemplJump:$<$ @/@>", args);
}

static i64 Templ_Print(Stream *sm, Abstract *a, cls type, word flags){
    status r = READY;

    Templ *templ = (Templ *)as(a, TYPE_TEMPL);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)templ, ToS_FlagLabels),
        (Abstract *)&templ->content,
        (Abstract *)&templ->data,
        NULL
    };
    return Fmt(sm, "Templ<$ content:@ data:@", args);
}

static i64 Node_Print(Stream *sm, Abstract *a, cls type, word flags){
    Node *nd = (Node*)as(a, TYPE_NODE);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)nd, ToS_FlagLabels),
        (Abstract *)Type_ToStr(sm->m, nd->typeOfChild),
        (Abstract *)Type_ToStr(sm->m, nd->captureKey),
        (Abstract *)(nd->parent != NULL ?
            Type_ToStr(sm->m, nd->parent->captureKey) : NULL),
        (Abstract *)nd->atts,
        (Abstract *)Type_ToStr(sm->m,
            (nd->value != NULL ? nd->value->type.of : _TYPE_ZERO)),
        (Abstract *)nd->value,
        (Abstract *)Type_ToStr(sm->m,
            (nd->child != NULL ? nd->child->type.of : _TYPE_ZERO)),
        NULL
    };
    if(flags & DEBUG){
        return Fmt(sm, "N<$/$ captureKey($) parent(@) atts:& valueTypeOf:$/@ childTypeOf:$>", args);
    }else{
        return Fmt(sm, "N<$/$ captureKey($) parent(@) atts:@ value:$/@ childTypeOf:$>", args);
    }
}

static i64 Relation_Print(Stream *sm, Abstract *a, cls type, word flags){
    status r = READY;
    i64 total = 0;
    Relation *rel = (Relation*)as(a, TYPE_RELATION);
    Abstract *args[] = {
        (Abstract *)I16_Wrapped(sm->m, rel->stride),
        (Abstract *)I32_Wrapped(sm->m, Relation_RowCount(rel)),
        (Abstract *)(rel->headers != NULL ?
            Ptr_Wrapped(sm->m, rel->headers, TYPE_ARRAY): NULL),
        NULL
    };
    total +=  Fmt(sm, "Rel<$x$ @ [", args);
    if(rel->it.p->max_idx >= 0){
        total += Stream_Bytes(sm, (byte *)"\n", 1);
    }

    while((Relation_Next(rel) & END) == 0){
        if(rel->type.state & RELATION_ROW_START){
            total += Stream_Bytes(sm, (byte *)"  ", 2);
        }
        total += ToS(sm, rel->it.value, 0, flags);
        if(rel->type.state & RELATION_ROW_END){
            total += Stream_Bytes(sm, (byte *)",\n", 2);
        }else{
            total += Stream_Bytes(sm, (byte *)",", 1);
        }
    }

    total += Stream_Bytes(sm, (byte *)"]>", 2);
    return total;
}

static i64 MessClimber_PrintItems(Stream *sm, MessClimber *climber, word flags, i64 total){
    i32 nested = ++climber->nested;
    if(climber->current != NULL){
        Abstract *current = climber->current;
        total += Stream_Bytes(sm, (byte *)"\n", 1);
        while(nested--){
            total += Stream_Bytes(sm, (byte *)"  ", 2);
        }
        if(current->type.of == TYPE_NODE){
            Node *nd = (Node *)climber->current;
            total += ToS(sm, (Abstract *)nd, 0, flags);
            if(nd->typeOfChild == TYPE_SPAN){
                total += Stream_Bytes(sm, (byte *)"[", 1);
                Iter it;
                Iter_Init(&it, (Span *)nd->child);
                while((Iter_Next(&it) & END) == 0){
                    climber->current = it.value;
                    total += MessClimber_PrintItems(sm, climber, flags, total);
                }
                total += Stream_Bytes(sm, (byte *)"]", 1);
            }else if(nd->typeOfChild == TYPE_NODE){
                climber->current = nd->child;
                total += Stream_Bytes(sm, (byte *)"<", 2);
                total += MessClimber_PrintItems(sm, climber, flags, total);
                total += Stream_Bytes(sm, (byte *)">", 1);
            }else{
                climber->current = nd->child;
                total += MessClimber_PrintItems(sm, climber, flags, total);
            }
        }else if(current->type.of == TYPE_SPAN){
            Iter it;
            Iter_Init(&it, (Span *)current);
            while((Iter_Next(&it) & END) == 0){
                climber->current = it.value;
                total += MessClimber_PrintItems(sm, climber, flags, total);
            }
        }else{
            total += ToS(sm, current, 0, MORE|DEBUG);
        }
        climber->current = current;
    }

    climber->nested--;
    return total;
}

static i64 Mess_Print(Stream *sm, Abstract *a, cls type, word flags){
    Mess *ms = (Mess *)as(a, TYPE_MESS);
    if((flags & (DEBUG|MORE)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }else{
        i64 total = 0;
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)ms, ToS_FlagLabels),
            NULL
        };
        total += Fmt(sm, "Mess<$", args);
        if(flags & DEBUG){
            Abstract *args[] = {
                (Abstract *)ms->current,
                (Abstract *)ms->currentValue,
                NULL
            };
            total += Fmt(sm, " current:$ value:@ [", args);
        }else{
            total += Stream_Bytes(sm, (byte *)" [", 2);
        }
        MessClimber climber = {
            .type = {TYPE_MESS_CLIMBER, 0},
            .nested = 1,
            .mess = ms,
            .current = (Abstract *)ms->root,
        };
        total += MessClimber_PrintItems(sm, &climber, flags|DEBUG, 0);
        total += Stream_Bytes(sm, (byte *)"\n]>", 3);
        return total;
    }
}

static i64 OrdTable_Print(Stream *sm, Abstract *a, cls type, word flags){
    OrdTable *otbl = (OrdTable *)as(a, TYPE_ORDTABLE);
    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, otbl->order->nvalues),
            (Abstract *)otbl->tbl,
            (Abstract *)otbl->order,
            NULL
        };
        return Fmt(sm, "OrdTable<^D.$^d.nvalues @/[@]>", args);
    }else if(flags & MORE){
        i64 total = 0;
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, otbl->order->nvalues),
            NULL
        };
        total += Fmt(sm, "OrdTable<^D.$^d.nvalues [", args);
        Iter it;
        Iter_Init(&it, otbl->order);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = (Hashed *)Iter_Get(&it);
            if(h != NULL){
                total += ToS(sm, h->key, 0, MORE);  
                total += Stream_Bytes(sm, (byte *)" -> ", 4);
                total += ToS(sm, h->value, 0, MORE);  
                if((it.type.state & LAST) == 0){
                    total += Stream_Bytes(sm, (byte *)", ", 2);
                }
            }
        }
        total += Stream_Bytes(sm, (byte *)"]>", 2);
        return total;
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

static i64 TemplItem_Print(Stream *sm, Abstract *a, cls type, word flags){
    Abstract *args[] = {
        (Abstract *)Type_ToStr(sm->m, type),
        (Abstract *)a,
        NULL
    };
    return Fmt(sm, "TemplItem:$<@>", args);
}

static i64 Frame_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    Frame *fm = (Frame *)as(a, TYPE_FRAME);
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(sm->m, fm->originIdx),
        (Abstract *)fm->originKey,
        (Abstract *)(fm->value != NULL ? Type_ToStr(sm->m, fm->value->type.of): NULL),
        (Abstract *)I32_Wrapped(sm->m, fm->it.idx),
        (Abstract *)Iter_Current(&fm->it),
        NULL,
    };
    total += Fmt(sm, "Frame<from:@/@ @[@]>", args);
    return total;
}

static i64 Nested_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    i16 guard = 0;
    Nested *_nd = (Nested *)as(a, TYPE_NESTED);
    Nested *nd = Nested_Make(sm->m);
    Nested_SetRoot(nd, Nested_GetRoot(_nd));
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)nd, ToS_FlagLabels),
        (Abstract *)&nd->it,
        NULL
    };
    total += Fmt(sm, "Nested<$ \\@@", args);
    if(flags & DEBUG){
        total = Stream_Bytes(sm, (byte *)"\n", 1);
        i32 indent = _nd->it.idx + 1;
        Iter it;
        while(TRUE){
            Guard_Incr(&guard, 32, FUNCNAME, FILENAME, LINENUMBER);
            Frame *fm = Iter_Current(&nd->it);
            while((Iter_Next(&fm->it) & END) == 0){
                Abstract *item = Iter_Get(&fm->it);
                if(item->type.of == TYPE_HASHED){
                    Hashed *h = (Hashed *)item;
                    total += indentStream(sm, indent);
                    total += ToS(sm, h->key, 0, MORE);
                    if(h->value->type.of == TYPE_ORDTABLE || h->value->type.of == TYPE_SPAN){
                        Nested_IndentByIdx(nd, fm->it.idx);
                        indent++;
                        total += Stream_Bytes(sm, (byte *)" ->\n", 4);
                        break;
                    }else{
                        total += Stream_Bytes(sm, (byte *)" -> ", 4);
                        total += ToS(sm, (Abstract *)h->value, 0, MORE);
                        if(fm->it.type.state & LAST){
                            total += Stream_Bytes(sm, (byte *)"\n", 1);
                        }else{
                            total += Stream_Bytes(sm, (byte *)",\n", 2);
                        }
                    }
                }else{
                    total += indentStream(sm, indent);
                    total += ToS(sm, (Abstract *)item, 0, MORE);
                    total += Stream_Bytes(sm, (byte *)",\n", 2);
                }
            }
            if(fm->it.type.state & END){
                indent--;
                if(nd->it.idx == 0){
                    break;
                }else{
                    Iter_Pop(&nd->it); 
                }
            }
        }
    }
    total += Stream_Bytes(sm, (byte *)">", 1);
    return total;
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

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Navigate_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_MESS, (void *)Mess_Print);
    r |= Lookup_Add(m, lk, TYPE_NODE, (void *)Node_Print);
    r |= Lookup_Add(m, lk, TYPE_TEMPL, (void *)Templ_Print);
    r |= Lookup_Add(m, lk, TYPE_TEMPL_JUMP, (void *)TemplJump_Print);
    r |= Lookup_Add(m, lk, TYPE_RELATION, (void *)Relation_Print);
    r |= Lookup_Add(m, lk, TYPE_COMP, (void *)Comp_Print);
    r |= Lookup_Add(m, lk, TYPE_COMPRESULT, (void *)CompResult_Print);
    r |= Lookup_Add(m, lk, TYPE_ORDTABLE, (void *)OrdTable_Print);
    r |= Lookup_Add(m, lk, FORMAT_CASH_VAR_KEY, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_CASH_VAR_FOR, (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_CASH_VAR_KEYVALUE, 
        (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, FORMAT_CASH_VAR_NAMEVALUE, 
        (void *)TemplItem_Print);
    r |= Lookup_Add(m, lk, TYPE_NESTED, (void *)Nested_Print);
    r |= Lookup_Add(m, lk, TYPE_FRAME, (void *)Frame_Print);
    return r;
}
