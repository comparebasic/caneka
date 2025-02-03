#include <external.h>
#include <caneka.h>

char *Class_ToString(cls type){
    if(type <= _TYPE_CORE_END){
       return TypeStrings[type]; 
    }else{
        return "TYPE_unknown";
    }
}

char *State_ToChars(status state){
    return String_ToChars(DebugM, State_ToString(DebugM, state));
}


void StrSnipString_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)asIfc(a, TYPE_STRING);
    IterStr it;
    IterStr_Init(&it, s, sizeof(StrSnip));
    printf("\x1b[%dm%s", color, msg);
    while((IterStr_Next(&it) & END) == 0){
        StrSnip *sn = (StrSnip *)IterStr_Get(&it);
        if((sn->type.state & STRSNIP_CONTENT) != 0){
            printf("%d/%d", sn->start, sn->length);
        }else if((sn->type.state & STRSNIP_UNCLAIMED) != 0){
            printf("unclaimed(%d/%d)", sn->start, sn->length);
        }else if((sn->type.state & STRSNIP_GAP) != 0){
            printf("gap(%d/%d)", sn->start, sn->length);
        }else{
            printf("%hd(%d/%d)", sn->type.state, sn->start, sn->length);
        }
        if((it.type.state & FLAG_ITER_LAST) == 0){
            printf(",");
        }else{
            printf("\x1b[0m");
        }
    }
}

void String_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_CHAIN);
    if(extended){
        char *state = "";
        if(s->type.state != 0){
            state = State_ToChars(s->type.state);
        }
        printf("%s\x1b[%dmS<%s\x1b[0;%dm", msg, color, state, color);
        do {
            int l = s->length;
            String *_s = s;
            if((s->type.state & FLAG_STRING_BINARY) != 0){
                _s = String_ToHex(DebugM, s);
            }else{
                _s = String_ToEscaped(DebugM, s);
            }
            printf("s/%u=\"\x1b[1;%dm", l, color);
            do {
                printf("%s", _s->bytes);
            } while((_s = String_Next(_s)) != NULL);
            printf("\x1b[0;%dm\"", color);
            s = s->next;
        } while(s != NULL);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%s\x1b[1;%dm",color, msg, color);
        do {
            /*
            printf("%d/%lu'%s'",s->length,strlen((char *)s->bytes),s->bytes);
            */
            printf("%s",s->bytes);
            s = s->next;
        } while(s != NULL);
        printf("\x1b[0m");
    }
}

void StringFixed_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_FIXED);
    String *esc = String_ToEscaped(DebugM, s);
    if(extended){
        char *state = "";
        if(s->type.state != 0){
            state = State_ToChars(s->type.state);
        }
        printf("%s\x1b[%dmSFixed<%s\x1b[0;%dm", msg, color,state, color);
        printf("s/%u=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, esc->bytes, color);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%s\x1b[1;%dm%s\x1b[0;%dm\x1b[0m", color, msg, color, esc->bytes, color);
    }
}

void StringFull_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_FULL);
    String *esc = String_ToEscaped(DebugM, s);
    if(extended){
        char *state = "";
        if(s->type.state != 0){
            state = State_ToChars(s->type.state);
        }
        printf("%s\x1b[%dmSFull<%s\x1b[0;%dm", msg, color,state, color);
        printf("s/%u=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, esc->bytes, color);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%s\x1b[1;%dm%s\x1b[0;%dm\x1b[0m", color, msg, color, esc->bytes, color);
    }
}

void Cstr_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    printf("\x1b[%dm%s%s\x1b[0m", color, msg, (char *)a);
}

void Cursor_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Cursor *cur = (Cursor *)as(a, TYPE_CURSOR);
    printf("\x1b[%dm%sCursor<%s ptr=\x1b[1;%dm'%c'\x1b[0;%dm offset=%ld local=%ld", color, msg,
        State_ToChars(cur->type.state), 
        color, (cur->ptr != NULL ? *(cur->ptr) : 0), color,
        cur->offset, cur->local);
    if(extended){
        MemCtx *m = DebugM;
        i64 total = Cursor_Total(cur);

        printf(" \x1b[%dmseg(%ld/%ld of %ld)=\x1b[1;%dm\"", color, max(total-36, 0), total, String_Length(cur->s), color);

        i64 start = 36;
        if(total < 36){
            start = total;
        }
        Debug_Print((void*)String_ToEscaped(m, String_Sub(m, cur->s, max(total-36, 0), start)), 0, "", color, FALSE);

        printf("\x1b[1;%d;37m%c\x1b[0;%dm", color+10, (cur->ptr != NULL ? *(cur->ptr) : 0), color);
        Debug_Print((void*)String_ToEscaped(m, String_Sub(m, cur->s, total+1, total+36)), 0, "", color, FALSE);
        printf("\x1b[1;%dm\"", color);
    }
    printf("\x1b[%dm>\x1b[0m", color);
}

