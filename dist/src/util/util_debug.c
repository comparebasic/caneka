#include <external.h>
#include <caneka.h>

static void Hashed_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(extended){
        printf("\x1b[%dm%sH<%s%u:%lu itm=", color, msg, State_ToChars(h->type.state), h->idx, h->id);
        Debug_Print((void *)h->item, 0, "", color, extended);
        printf("\x1b[%dm v=", color);
        Debug_Print((void *)h->value, 0, "", color, extended);
        if(h->next != NULL){
            printf("\x1b[%dm next=%p", color, h);
        }
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sH<", color, msg);
        Debug_Print((void *)h->item, 0, "", color, FALSE);
        if(h->value != NULL){
            printf("\x1b[%dm v=", color);
            Debug_Print((void *)h->value, 0, "", color, FALSE);
        }
        printf("\x1b[%dm>\x1b[0m", color);
    }
}

static void Wrapped_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)asIfc(a, TYPE_WRAPPED);
    printf("\x1b[%dm%sW<%s>\x1b[0m", color, msg, Class_ToString(a->type.of));
}

static void WrappedUtil_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_UTIL);
    printf("\x1b[%dm%sWi64<\x1b[1;%dm%lu\x1b[0;%dm>\x1b[0m", color,  msg, color, sgl->val.value, color);
}

static void WrappedI64_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_I64);
    printf("\x1b[%dm%sWi64<\x1b[1;%dm%lu\x1b[0;%dm>\x1b[0m", color,  msg, color, sgl->val.value, color);
}

static void WrappedTime64_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_TIME64);
    String *s = Time64_Present(DebugM, a);
    printf("\x1b[%dm%sT64<\x1b[1;%dm%s\x1b[0;%dm>\x1b[0m", color, msg, color, s->bytes, color);
}

static void Abstract_Print(Abstract *t, cls type, char *msg, int color, boolean extended){
    if(t == NULL){
        printf("u0");
    }else{
        printf("%p", t);
    }
}


static void mess_PrintRecurse(Mess *ms, char *msg, int color, boolean extended, int indent){
    indent_Print(indent);
    printf("\x1b[%dm%sM<%s value=\x1b[0m", color, msg, Class_ToString(ms->type.of));
    Debug_Print((void *)ms->name, 0, "", color, extended);
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
}

static void Mess_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Mess *ms = (Mess *)a;
    mess_PrintRecurse(ms, msg, color, extended, 0);
}

static void Single_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *v = (Single *)a;
    printf("\x1b[%dm%sSingle<%s:%u:%ld>\x1b[0m", color, msg, Class_ToString(v->type.of), v->type.state, v->val.value);
}

static void Result_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Result *v = (Result *)a;
    printf("\x1b[%dm%sR<%s:\x1b[1;%dm%d\x1b[0;%dm:", color, msg, State_ToChars(v->type.state), color, v->range, color);
    Debug_Print((void *)v->s, 0, "", color, FALSE);
    printf("\x1b[%dm>\x1b[0m", color);
}
