#include <external.h>
#include <caneka.h>

static boolean crashing = FALSE;

void *Fatal(i32 fd, char *func, char *file, int line, char *fmt, void **args){
#ifdef CLI    
    RawMode(FALSE);
#endif
    void *args1[] = { func, file, &line, NULL};
    StrVec_Fmt(ErrStream, "^r.Fatal Error: _c:_c:_i4 ^D", args1);
    StrVec_FmtHandle(ErrStream, fmt, args);
    StrVec_Fmt(ErrStream, "^d", NULL);
#ifdef OPENSSL
    char _buff[256];
    unsigned long e = ERR_get_error();
    if(e != 0){
        char *openssl_err = ERR_error_string(e, _buff);
        StrVec_Fmt(ErrStream, "^rD^_T^0", NULL);
        void *_args2[] = {openssl_err, &TYPE_CSTR, NULL};
        StrVec_Fmt(ErrStream, "  ^rD^_T^0", openssl_err, _args2);
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
