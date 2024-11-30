#include <external.h>
#include <caneka.h>

#define TIME_BUFF_LEN 42

String *Time64_Present(MemCtx *m, Abstract *a){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_TIME64);
    String *s = String_Init(m, TIME_BUFF_LEN);
    struct timespec ts;
    struct tm tm;
    Time64_ToSpec(&ts, (time64_t)sg->val.value);

    gmtime_r(&ts.tv_sec, &tm);
    strftime((char *)s->bytes, TIME_BUFF_LEN, "%Y-%m-%dT%H:%M:%S.", &tm);
    s->length = strlen((char *)s->bytes);
    String_AddI64(m, s, ts.tv_nsec);
    String_AddBytes(m, s, bytes("+00"), 3);
    return s;
}

String *I64_Present(MemCtx *m, Abstract *s){
    Single *sg = (Single *)as(s, TYPE_WRAPPED_I64);
    return NULL;
}

String *Util_Present(MemCtx *m, Abstract *s){
    Single *sg = (Single *)as(s, TYPE_WRAPPED_UTIL);
    return NULL;
}

String *Bool_Present(MemCtx *m, Abstract *s){
    Single *sg = (Single *)as(s, TYPE_WRAPPED_BOOL);
    return NULL;
}

String *String_Present(MemCtx *m, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
    String *ret = Buff(m);
    boolean in = FALSE;
    if((s->type.state & FLAG_STRING_TEXT) == 0){
        while(s != NULL){
            for(int i = 0; i < s->length; i++){
                byte *b = s->bytes+i;
                if(IS_VISIBLE(*b)){
                    if(in){
                        String_AddBytes(m, ret, bytes("}"), 1);
                        in = FALSE;
                    }
                    String_AddBytes(m, ret, b, 1);
                }else{
                    if(!in){
                        String_AddBytes(m, ret, bytes("\\{"), 2);
                        in = TRUE;
                    }else{
                        String_AddBytes(m, ret, bytes(","), 1);
                    }
                    int n = *b;
                    String_AddInt(m, ret, n);
                }
            }

            s = String_Next(s);
        }
        if(in){
            String_AddBytes(m, ret, bytes("}"), 1);
            in = FALSE;
        }
    }else{
        ret = s;
    }
    return ret;
}
