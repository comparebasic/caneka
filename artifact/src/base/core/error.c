#include <external.h>
#include <caneka.h>

static boolean crashing = FALSE;

void *Fatal(char *func, char *file, int line, char *fmt, Abstract *args[]){
#ifdef CLI    
    RawMode(FALSE);
#endif
    Abstract *args1[] = {
        (Abstract *)Str_CstrRef(ErrStream->m, func),
        (Abstract *)Str_CstrRef(ErrStream->m, file),
        (Abstract *)I32_Wrapped(ErrStream->m, line),
        NULL
    };
    Fmt(ErrStream, "^r.Fatal Error: $:$:$ ^D", args1);
    Fmt(ErrStream, fmt, args);
    Fmt(ErrStream, "^0.", NULL);
#ifdef OPENSSL
    char _buff[256];
    unsigned long e = ERR_get_error();
    if(e != 0){
        char *openssl_err = ERR_error_string(e, _buff);
        Abstract *args2[] = {
            (Abstract *)Str_CstrRef(ErrStream->m, openssl_err),
            NULL
        };
        Fmt(ErrStream, "^rD^$^0", args2);
    }
#endif
    Stream_Bytes(ErrStream, (byte *)"\n", 1);
    if(!crashing){
        crashing = TRUE;
        DebugStack_Print();
    }
    exit(13);
    return NULL;
}
