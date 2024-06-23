#include "external.h"
#include "filestore.h"

static void Match_PrintPat(Match *mt, char *msg, int color, boolean extended){
    if(extended){
        printf("%sMatch<%s:\x1b[%d;1mstate=%s:pos=%d:val=%d ", msg, Class_ToString(mt->type), color, State_ToString(mt->state), mt->position, mt->intval);
        int length = mt->s->length / sizeof(PatCharDef);
        PatCharDef *def = (PatCharDef *)mt->s;
        for(int i = 0; i < length;i++){
            if((def->flags & PAT_COUNT) != 0){
                printf("%u-'%c(%hu)',", def->flags, def->from, def->to);
            }else{
                printf("%u-'%c/%c',", def->flags, def->from, def->to);
            }
            def++;
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
        printf("%sMatch<%s:\x1b[%d;1m%s\x1b[0;%dm:state=%s:pos=%d:val=%d>\n", msg, Class_ToString(mt->type), color, mt->s->bytes, color, State_ToString(mt->state), mt->position, mt->intval);
    }else{
        printf("%sMatch<state=%s:pos=%d>\n", msg, mt->s->bytes, mt->position);
    }
}

static void String_Print(String *s, char *msg, int color, boolean extended){
    printf("%s\x1b[1;%dmS<\x1b[0;1m", msg, color);
    do {
        printf("=\"\x1b[1;%dm%ld:%s\x1b[0;1m\"", color, s->length, s->bytes);
        s = s->next;
    } while(s != NULL);
    printf("\x1b[1;%dm>\x1b[0m\n", color);
}

static void Req_Print(Req *req, char *msg, int color, boolean extended){
    printf("%s\x1b[1;%dmReq<%s:%s %s:%s,%s,%s>\x1b[0;1m\n",
        msg, color, State_ToString(req->state), Method_ToString(req->method),
        req->path != NULL ? (char *)req->path->bytes : "",
        req->body != NULL ? "body" : "no-body",
        req->response != NULL ? "response" : "no-response",
        req->cursor != NULL ? "cursor" : "no-cursor"
    );
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
    if(t == NULL){
        printf("NULL");
    }else if(type == TYPE_MATCH){
        Match_Print((Match *)t, msg, color, extended);
    }else if(type == TYPE_STRING){
        String_Print((String *)t, msg, color, extended);
    }else if(type == TYPE_SCURSOR){
        SCursor_Print((SCursor *)t, msg, color, extended);
    }else if(type == TYPE_RANGE){
        Range_Print((Range *)t, msg, color, extended);
    }else if(type == TYPE_REQ){
        Req_Print((Req *)t, msg, color, extended);
    }else{
        printf("%s: %s unkown debug", msg, Class_ToString(type));
    }
    if(color >= 0){
        printf("\x1b[0m");
    }
    fflush(stdout);
}
