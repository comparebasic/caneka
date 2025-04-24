#include <external.h>
#include <caneka.h>

static boolean crashing = FALSE;

void *Fatal(char *func, char *file, int line, char *fmt, Single args[]){
#ifdef CLI    
    RawMode(FALSE);
#endif
    Single args1[] = {
        {{TYPE_WRAPPED_CSTR, 0}, .val.ptr = func},
        {{TYPE_WRAPPED_CSTR, 0}, .val.ptr = file},
        {{TYPE_WRAPPED_I32, 0}, .val.i = line}
    };
    StrVec_Fmt(ErrStream, "^r.Fatal Error: $:$:$ ^D", args1);
    StrVec_FmtHandle(ErrStream, fmt, args);
    StrVec_Fmt(ErrStream, "^0.", NULL);
#ifdef OPENSSL
    char _buff[256];
    unsigned long e = ERR_get_error();
    if(e != 0){
        char *openssl_err = ERR_error_string(e, _buff);
        Single args2[] = {
            {{TYPE_WRAPPED_CSTR, 0}, .val.ptr = openssl_err}
        };
        StrVec_Fmt(ErrStream, "^rD^$^0", args2);
    }
#endif
    Stream_To(ErrStream, "\n", 1);
    if(!crashing){
        crashing = TRUE;
        DebugStack_Print();
    }
    exit(13);
    return NULL;
}

void *Error(char *msg){
    printf("Error: %s\n", msg);
    return NULL;
}
