#include <external.h>
#include <filestore.h>

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

static void PatCharDef_Print(PatCharDef *def, char *msg, int color, boolean extended){
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


static void Match_PrintPat(Match *mt, char *msg, int color, boolean extended){
    if(extended){
        printf("%sMatch<%s:state=%s:pos=%d:val=%d \x1b[%d;1m", msg, Class_ToString(mt->type), State_ToString(mt->state), mt->position, mt->intval, color);
        int length = mt->s->length / sizeof(PatCharDef);
        PatCharDef *def = (PatCharDef *)mt->s->bytes;
        for(int i = 0; i < length;i++){
            PatCharDef_Print(def++, "", color, extended);
        }
        printf(">\x1b[0m\n");
    }else{
        printf("%sMatch<state=%s:pos=%d>\n", msg, mt->s->bytes, mt->position);
    }
}

static void Match_Print(Match *mt, char *msg, int color, boolean extended){
    if(mt->type == TYPE_PATMATCH){
        return Match_PrintPat(mt, msg, color, extended);
    }

    if(extended){
        printf("%sMatch<%s:state=%s:pos=%d:'\x1b[%d;1m%s\x1b[0;%dm':val=%d>\n", msg, Class_ToString(mt->type), State_ToString(mt->state), mt->position, color, mt->s->bytes, color, mt->intval);
    }else{
        printf("%sMatch<state=%s:pos=%d>\n", msg, State_ToString(mt->state), mt->position);
    }
}

static void String_Print(String *s, char *msg, int color, boolean extended){
    printf("%s\x1b[%dmS<\x1b[0;%dm", msg, color, color);
    do {
        printf("s/%hu=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, s->bytes, color);
        s = s->next;
    } while(s != NULL);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void Req_Print(Req *req, char *msg, int color, boolean extended){
    printf("%s\x1b[1;%dmReq<%s:%s %s:%s,%s,%s>\x1b[0;1m\n",
        msg, color, State_ToString(req->state), Method_ToString(req->in.method),
        req->in.path != NULL ? (char *)req->in.path->bytes : "",
        req->in.body != NULL ? "body" : "no-body",
        req->out.response != NULL ? "response" : "no-response",
        req->out.cursor != NULL ? "cursor" : "no-cursor"
    );
}

static void Unit_Print(Unit *t, char *msg, int color, boolean extended){
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
        Unit *t = slab->items[i];
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

static void Slab_Print(Slab *slab, char *msg, int color, boolean extended){
    if(slab->increment != SPAN_DIM_SIZE){
        return slab_Summarize(slab, msg, color, extended);
    }

    printf("%s\x1b[%dmL<icr%d[%d] \x1b[%dm", msg, color, slab->increment, slab->offset, color);
    boolean first = TRUE;
    for(int i = 0; i < SPAN_DIM_SIZE; i++){
        Unit *t = slab->items[i];
        if(t != NULL){
            if(!first){
                printf(", ");
            }
            if(first){
                first = FALSE;
            }
            printf("%d=", i);
            Debug_Print((void *)t, TYPE_UNIT, "", color, extended); 

            printf("\x1b[%dm", color);
        }
    }
    printf("\x1b[%dm>\x1b[0m", color);
}

static void showSlab(Slab *sl, int color, boolean extended, int indent){
    Slab_Print(sl, "", color, extended); 
    if(sl->increment > SPAN_DIM_SIZE){
        indent++;
        printf("\n");
        boolean first = TRUE;
        for(int i = 0; i < SPAN_DIM_SIZE; i++){
            Unit *t = sl->items[i];
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

static void Span_Print(Span *p, char *msg, int color, boolean extended){
    printf("%s\n\x1b[;%dmP<%u items in %u dims of %lu bytes each", msg, color, 
        p->nvalues, p->dims, sizeof(Unit *)*p->slotSize);
    printf("\n");
    indent_Print(1);
    showSlab(p->slab, color, extended, 1);
    printf("\n\x1b[1;%dm>\x1b[0m\n", color);
}

static void SCursor_Print(SCursor *sc, char *msg, int color, boolean extended){
    printf("%s\x1b[1;%dmCursor<%s:%ld/seg%ld[%ld]:%ld>\x1b[0;1m", msg, color,
        State_ToString(sc->state), 
        sc->position, sc->segIdx, sc->localPosition, sc->immidiateLength
    );
}

static void Range_Print(Range *range, char *msg, int color, boolean extended){
    printf("%s\x1b[1;%dmReq<%s ", msg, color, State_ToString(range->state));
    Debug_Print((void *)&(range->start), TYPE_SCURSOR, "", color, extended);
    Debug_Print((void *)&(range->end), TYPE_SCURSOR, " -> ", color, extended);
    printf("\x1b[1;>\n");
}

void Debug_Print(void *t, cls type, char *msg, int color, boolean extended){
    if(color >= 0){
        printf("\x1b[%dm", color);
    }

    if(type == TYPE_UNIT){
        if(t == NULL){
            printf("%s\x1b[%dm0\x1b[0m", msg, color);
            return;
        }else{
            Unit *u = (Unit *)t;
            type = u->type.of;
        }
    }
    if(t == NULL){
        printf("NULL");
    }else if(type == TYPE_MATCH || type == TYPE_STRINGMATCH ){
        Match_Print((Match *)t, msg, color, extended);
    }else if(type == TYPE_PATCHARDEF){
        PatCharDef_Print((PatCharDef *)t, msg, color, extended);
    }else if( type == TYPE_PATMATCH ){
        Match_PrintPat((Match *)t, msg, color, extended);
    }else if(type == TYPE_STRING_CHAIN || type == TYPE_STRING_FIXED){
        String_Print((String *)t, msg, color, extended);
    }else if(type == TYPE_SCURSOR){
        SCursor_Print((SCursor *)t, msg, color, extended);
    }else if(type == TYPE_RANGE){
        Range_Print((Range *)t, msg, color, extended);
    }else if(type == TYPE_REQ){
        Req_Print((Req *)t, msg, color, extended);
    }else if(type == TYPE_SLAB){
        Slab_Print((Slab *)t, msg, color, extended);
    }else if(type == TYPE_SPAN){
        Span_Print((Span *)t, msg, color, extended);
    }else{
        printf("%s: %s unkown debug", msg, Class_ToString(type));
    }
    if(color >= 0){
        printf("\x1b[0m");
    }
    fflush(stdout);
}
