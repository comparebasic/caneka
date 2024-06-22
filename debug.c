#include "external.h"
#include "filestore.h"

static void Match_Print(Match *mt, char *msg, int color, boolean extended){
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
    printf("%s\x1b[1;%dmReq<%s:%s/%s:%s,%s,%s>\x1b[0;1m\n",
        msg, color, State_ToString(req->state), Method_ToString(req->method),
        req->path != NULL ? (char *)req->path->bytes : "",
        req->body != NULL ? "body" : "no-body",
        req->response != NULL ? "response" : "no-response",
        req->cursor != NULL ? "cursor" : "no-cursor"
    );
}

void Debug_Print(void *t, cls type, char *msg, int color, boolean extended){
    if(color >= 0){
        printf("\x1b[%dm", color);
    }
    if(type == TYPE_MATCH){
        Match_Print((Match *)t, msg, color, extended);
    }else if(type == TYPE_STRING){
        String_Print((String *)t, msg, color, extended);
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
