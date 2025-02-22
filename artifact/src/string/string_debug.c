#include <external.h>
#include <caneka.h>

void StrSnipString_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)asIfc(a, TYPE_STRING);
    IterStr it;
    IterStr_Init(&it, s, sizeof(StrSnip), NULL);
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
            _s = String_ToEscaped(DebugM, s);
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
            if(s->type.state & FLAG_STRING_BINARY){
                printf("\x1b[%dmS<Binary/%d>\x1b[0m", color, s->length);
            }else{
                printf("%s",s->bytes);
            }
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
    DebugStack_Push("Cursor_Print", TYPE_CSTR);
    Cursor *cur = (Cursor *)as(a, TYPE_CURSOR);
    String *s = String_Init(DebugM, 1);
    String *focus = NULL;
    if(cur->ptr != NULL){
        String_AddBytes(DebugM, s, cur->ptr, 1);
        focus = String_ToEscaped(DebugM, s);
    }
    printf("\x1b[%dm%sCursor<%s ptr=\x1b[1;%dm'%s'\x1b[0;%dm offset=%ld local=%ld", color, msg,
        State_ToChars(cur->type.state), 
        color, (focus != NULL ? (char *)focus->bytes : "?"), color,
        cur->offset, cur->local);
    if(extended){
        MemCtx *m = DebugM;
        i64 total = Cursor_Total(cur);

        printf(" \x1b[%dmseg(%ld/%ld of %ld)=\x1b[1;%dm\"", color, max(total-36, 0), total, String_Length(cur->s), color);
        if(cur->s != NULL && cur->s->length > 0){
            i64 start = 36;
            if(total < 36){
                start = total;
            }
            Debug_Print((void*)String_ToEscaped(m, String_Sub(m, cur->s, max(total-36, 0), start)), 0, "", color, FALSE);

            printf("\x1b[1;%d;37m%s\x1b[0;%dm", color+10, (focus != NULL ? (char *)focus->bytes: "?"), color);
            Debug_Print((void*)String_ToEscaped(m, String_Sub(m, cur->s, total+1, total+36)), 0, "", color, FALSE);
            printf("\x1b[1;%dm\"", color);
        }
    }
    printf("\x1b[%dm>\x1b[0m", color);
    DebugStack_Pop();
    return;
}

status StringDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING_CHAIN, (void *)String_Print);
    r |= Lookup_Add(m, lk, TYPE_STRING_FIXED, (void *)StringFixed_Print);
    r |= Lookup_Add(m, lk, TYPE_STRING_FULL, (void *)StringFull_Print);
    r |= Lookup_Add(m, lk, TYPE_CURSOR, (void *)Cursor_Print);
    r |= Lookup_Add(m, lk, TYPE_STRSNIP_STRING, (void *)StrSnipString_Print);
    r |= Lookup_Add(m, lk, TYPE_CSTR, (void *)Cstr_Print);
    return r;
}
