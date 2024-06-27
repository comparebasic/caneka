#include <external.h>
#include <caneka.h>

Chain *DebugPrintChain = NULL;

int DEBUG_SCURSOR = COLOR_DARK;
int DEBUG_MATCH = 0;
int DEBUG_PATMATCH = 0;
int DEBUG_ALLOC = 0;
int DEBUG_BOUNDS_CHECK = 0;

static void indent_Print(int indent){
    while(indent--){
        printf("  ");
    }
}

static void PatCharDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    PatCharDef *def = (PatCharDef *) as(a, TYPE_PATCHARDEF);
    if((def->flags & PAT_COUNT) != 0){
        if(def->from == '\r' || def->from == '\n'){
            printf("%s%hu=%hux%hu,", msg, (word)def->flags, def->from, def->to);
        }else{
            printf("%s%hu=%cx%hu,", msg, (word)def->flags, (char)def->from, def->to);
        }
    }else if(def->from == def->to){
        if(def->from == '\r' || def->from == '\n' || def->from == '\t'){
            printf("%s%hu=%hu,", msg, (word)def->flags, def->from);
        }else{
            printf("%s%hu=%c,", msg, (word)def->flags, (char)def->from);
        }
    }else{
        if((def->from == '\r' || def->from == '\n') || (def->to == '\r' || def->to == '\n')){
            printf("%s%hu=%hu-%hu,", msg, (word)def->flags, def->from, def->to);
        }else{
            printf("%s%hu=%c-%c,", msg, (word)def->flags, (char)def->from, (char)def->to);
        }
    }
}

static void Match_PrintPat(Abstract *a, cls type, char *msg, int color, boolean extended){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    if(extended){
        printf("%sMatch<%s:state=%s:pos=%d:val=%d \x1b[%d;1m", msg, Class_ToString(mt->type), State_ToString(mt->state), mt->position, mt->intval, color);
        int length = mt->s->length / sizeof(PatCharDef);
        PatCharDef *def = (PatCharDef *)mt->s->bytes;
        for(int i = 0; i < length;i++){
            PatCharDef_Print((Abstract *)def++, type, "", color, extended);
        }
        printf(">\x1b[0m\n");
    }else{
        printf("%sMatch<state=%s:pos=%d>\n", msg, mt->s->bytes, mt->position);
    }
}

static void Match_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Match *mt = (Match *)as(a, TYPE_MATCH);
    if(extended){
        printf("%sMatch<%s:state=%s:pos=%d:'\x1b[%d;1m%s\x1b[0;%dm':val=%d>\n", msg, Class_ToString(mt->type), State_ToString(mt->state), mt->position, color, mt->s->bytes, color, mt->intval);
    }else{
        printf("%sMatch<state=%s:pos=%d>\n", msg, State_ToString(mt->state), mt->position);
    }
}
static void StringFixed_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_FIXED);
    printf("%s\x1b[%dmSFixed<\x1b[0;%dm", msg, color, color);
    do {
        printf("s/%hu=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, s->bytes, color);
        s = s->next;
    } while(s != NULL);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void String_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    printf("String Print Called %s\n", Class_ToString(type));
    String *s = (String *)as(a, TYPE_STRING_CHAIN);
    printf("%s\x1b[%dmS<\x1b[0;%dm", msg, color, color);
    do {
        printf("s/%hu=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, s->bytes, color);
        s = s->next;
    } while(s != NULL);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void Req_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Req *req = (Req *) as(a, TYPE_REQ);
    printf("%s\x1b[1;%dmReq<%s:%s>\x1b[0;1m\n",
        msg, color, State_ToString(req->state), Proto_ToChars(req->proto));
}

static void Abstract_Print(Abstract *t, cls type, char *msg, int color, boolean extended){
    if(t == NULL){
        printf("u0");
    }else{
        printf("%p", t);
    }
}

static void slab_Summarize(Slab *slab, char *msg, int color, boolean extended){
    printf("%s\x1b[0;%dmL<incr%d[%d] ", msg, color, slab->increment, slab->offset);
    boolean first = TRUE;
    for(int i = 0; i < SPAN_DIM_SIZE; i++){
        Abstract *t = slab->items[i];
        if(t != NULL){
            if(!first){
                printf(", ");
            }
            if(first){
                first = FALSE;
            }
            printf("%d=", i);
            if(t->type.of == TYPE_SLAB){
                Slab *slt = (Slab *)t;
                printf("%u", slt->offset);
            }else{
                printf("%p", t);
            }
            printf("\x1b[%dm", color);
        }
    }
    printf("\x1b[1;%dm>\x1b[0m", color);
}

static void Slab_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Slab *slab = (Slab *)a;
    if(slab->increment != SPAN_DIM_SIZE){
        return slab_Summarize(slab, msg, color, extended);
    }

    printf("%s\x1b[%dmL<icr%d[%d] \x1b[%dm", msg, color, slab->increment, slab->offset, color);
    boolean first = TRUE;
    for(int i = 0; i < SPAN_DIM_SIZE; i++){
        Abstract *t = slab->items[i];
        if(t != NULL){
            if(!first){
                printf(", ");
            }
            if(first){
                first = FALSE;
            }
            printf("%d=", i);
            Debug_Print((void *)t, 0, "", color, extended); 

            printf("\x1b[%dm", color);
        }
    }
    printf("\x1b[%dm>\x1b[0m", color);
}

static void showSlab(Slab *sl, int color, boolean extended, int indent){
    Slab_Print((Abstract *)sl, TYPE_SLAB, "", color, extended); 
    if(sl->increment > SPAN_DIM_SIZE){
        indent++;
        printf("\n");
        boolean first = TRUE;
        for(int i = 0; i < SPAN_DIM_SIZE; i++){
            Abstract *t = sl->items[i];
            if(t != NULL){
                if(!first){
                    printf("\n");
                }
                if(first){
                    first = FALSE;
                }
                indent_Print(indent);
                printf("\x1b[%dm%d=", color, i);
                showSlab((Slab *)t, color, extended, indent);
            }
        }
    }
}

static void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Span *p = (Span *)as(a, TYPE_SPAN);
    printf("%s\n\x1b[;%dmP<%u items in %u dims of %lu bytes each", msg, color, 
        p->nvalues, p->dims, sizeof(Abstract *)*p->slotSize);
    printf("\n");
    indent_Print(1);
    showSlab(p->slab, color, extended, 1);
    printf("\n\x1b[1;%dm>\x1b[0m\n", color);
}

static void SCursor_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    SCursor *sc = (SCursor *)a;
    printf("%s\x1b[1;%dmCursor<%s:%ld/seg%ld[%ld]:%ld>\x1b[0;1m", msg, color,
        State_ToString(sc->state), 
        sc->position, sc->segIdx, sc->localPosition, sc->immidiateLength
    );
}

static void Range_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Range *range = (Range *)a;
    printf("%s\x1b[1;%dmReq<%s ", msg, color, State_ToString(range->state));
    Debug_Print((void *)&(range->start), TYPE_SCURSOR, "", color, extended);
    Debug_Print((void *)&(range->end), TYPE_SCURSOR, " -> ", color, extended);
    printf("\x1b[1;>\n");
}

static status populateDebugPrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Abstract_Print);
    r |= Lookup_Add(m, lk, TYPE_MATCH, (void *)Match_Print);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)PatCharDef_Print);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_STRING_CHAIN, (void *)String_Print);
    r |= Lookup_Add(m, lk, TYPE_STRING_FIXED, (void *)StringFixed_Print);
    r |= Lookup_Add(m, lk, TYPE_SCURSOR, (void *)SCursor_Print);
    r |= Lookup_Add(m, lk, TYPE_RANGE, (void *)Range_Print);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Req_Print);
    r |= Lookup_Add(m, lk, TYPE_SLAB, (void *)Slab_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    return r;
}

status Debug_Init(MemCtx *m){
    if(DebugPrintChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateDebugPrint, NULL);
        DebugPrintChain = Chain_Make(m, funcs);
        return SUCCESS;
    }
    return NOOP;
}

void Debug_Print(void *t, cls type, char *msg, int color, boolean extended){
    if(color >= 0){
        printf("\x1b[%dm", color);
    }

    if(t == NULL){
        printf("%s\x1b[%dmNULL\x1b[0m", msg, color);
        return;
    }

    Abstract *a = (Abstract *)t;
    if(type == 0){
        a = (Abstract *)t;
        type = a->type.of;
    }

    DebugPrintFunc func = (DebugPrintFunc)Chain_Get(DebugPrintChain, type);
    if(func != NULL){
        return func(a, type, msg, color, extended);
    }else{
        printf("%s: %s unkown debug", msg, Class_ToString(type));
    }

    if(color >= 0){
        printf("\x1b[0m");
    }
    fflush(stdout);
}
