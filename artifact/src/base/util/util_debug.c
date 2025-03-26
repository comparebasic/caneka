#include <external.h>
#include <caneka.h>

static i64 Hashed_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    /*
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
        printf("\x1b[%dm%sH<%s ", color, msg, State_ToChars(h->type.state));
        Debug_Print((void *)h->item, 0, "k=", color, FALSE);
        if(h->value != NULL){
            printf("\x1b[%dm v=", color);
            Debug_Print((void *)h->value, 0, "", color, FALSE);
        }
        printf("\x1b[%dm>\x1b[0m", color);
    }
    */
    return 0;
}

static i64 Wrapped_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    /*
    String *s = (String *)asIfc(a, TYPE_WRAPPED);
    printf("\x1b[%dm%sW<%s>\x1b[0m", color, msg, Class_ToString(a->type.of));
    */
    return 0;
}

static i64 WrappedUtil_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_UTIL);
    /*
    printf("\x1b[%dm%sWi64<\x1b[1;%dm%lu\x1b[0;%dm>\x1b[0m", color,  msg, color, sgl->val.value, color);
    */
    return 0;
}

static i64 WrappedI64_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_I64);
    /*
    printf("\x1b[%dm%sWi64<\x1b[1;%dm%lu\x1b[0;%dm>\x1b[0m", color,  msg, color, sgl->val.value, color);
    */
    return 0;
}

static i64 WrappedTime64_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_TIME64);
    /*
    String *s = Time64_Present(DebugM, a);
    printf("\x1b[%dm%sT64<\x1b[1;%dm%s\x1b[0;%dm>\x1b[0m", color, msg, color, s->bytes, color);
    */
    return 0;
}

static i64 Abstract_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    /*
    if(t == NULL){
        printf("u0");
    }else{
        printf("%p", t);
    }
    */
    return 0;
}

static i64 Single_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    /*
    Single *v = (Single *)a;
    printf("\x1b[%dm%sSingle<%s:%u:%ld>\x1b[0m", color, msg, Class_ToString(v->type.of), v->type.state, v->val.value);
    */
    return 0;
}

status UtilDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Print);
    r |= Lookup_Add(m, lk, TYPE_SINGLE, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_UTIL, (void *)WrappedUtil_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I64, (void *)WrappedI64_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_TIME64, (void *)WrappedTime64_Print);
    return r;
}
