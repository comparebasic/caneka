#include <external.h>
#include <caneka.h>

Lookup *ErrorHandlers = NULL;
boolean _crashing = FALSE;
boolean _error = FALSE;

void Fatal(char *func, char *file, int line, char *fmt, Abstract *args[]){
    if(_crashing){
        Stream_Bytes(ErrStream, (byte *)"\n\x1b[1;31mFatal called after crashing\x1b[0m", 39);
        exit(9);
        return;
    }
#ifdef CLI 
    RawMode(FALSE);
#endif
    _crashing = TRUE;
    Stream_Bytes(ErrStream, (byte *)"\n\x1b[22;31m", 9);
    Stream_Bytes(ErrStream, (byte *)"Fatal Error:\x1b[1m", 16);
    Stream_Bytes(ErrStream, (byte *)func, strlen(func));
    Stream_Bytes(ErrStream, (byte *)"\x1b[22m:", 6);
    Stream_Bytes(ErrStream, (byte *)file, strlen(file));
    Stream_Bytes(ErrStream, (byte *)":", 1);
    byte lineNo[MAX_BASE10+1];
    byte *b = lineNo;
    i64 length = Str_I64OnBytes(&b, line);
    Stream_Bytes(ErrStream, (byte *)b, length);
    Stream_Bytes(ErrStream, (byte *)" \x1b[1m", 5);
    Fmt(ErrStream, fmt, args);
    Stream_Bytes(ErrStream, (byte *)"\x1b[0m", 4);
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
    DebugStack_Print(ErrStream, 0);
    exit(13);
}

boolean IsZeroed(byte *b, size_t sz, char *func, char *file, int line){
    while(sz >= sizeof(util)){
        util *bu = (util *)b;
        if(*bu != 0){
            goto err;
        }
        sz -= sizeof(util);
        b += sizeof(util);
    }
    if(sz > 0){
        while(--sz > 0){
            if(*b != 0){
                goto err;
            }
            b++;
        }
    }

    return TRUE;
err:
    Error(ErrStream->m, NULL, func, file, line, "Memory not Zeroed", NULL);
    return FALSE;
}

void Error(MemCh *m, Abstract *a, char *func, char *file, int line, char *fmt, Abstract *args[]){
    _error = TRUE;
    status r = READY;
    if(a != NULL){
        DoFunc func = (DoFunc)Lookup_Get(ErrorHandlers, a->type.of);
        if(func != NULL){
           r |= func(m, a); 
        }
    }
    if((r & (SUCCESS|ERROR)) != 0){
        ShowError(func, file, line, fmt, args);
    }
    _error = FALSE;
    return;
}

void ShowError(char *func, char *file, int line, char *fmt, Abstract *args[]){
    if(_crashing){
        return Fatal(func, file, line, fmt, args);
    }
    _crashing = TRUE;
#ifdef CLI 
    RawMode(FALSE);
#endif
    Stream_Bytes(ErrStream, (byte *)"\n\x1b[22;31m", 9);
    Stream_Bytes(ErrStream, (byte *)"Error:\x1b[1m", 10);
    Stream_Bytes(ErrStream, (byte *)func, strlen(func));
    Stream_Bytes(ErrStream, (byte *)func, strlen(func));
    Stream_Bytes(ErrStream, (byte *)"\x1b[22m:", 6);
    Stream_Bytes(ErrStream, (byte *)file, strlen(file));
    Stream_Bytes(ErrStream, (byte *)":", 1);
    byte lineNo[MAX_BASE10+1];
    byte *b = lineNo;
    i64 length = Str_I64OnBytes(&b, line);
    Stream_Bytes(ErrStream, (byte *)b, length);
    Stream_Bytes(ErrStream, (byte *)" \x1b[1m", 5);
    Fmt(ErrStream, fmt, args);
    Stream_Bytes(ErrStream, (byte *)"\x1b[0m", 4);
    Stream_Bytes(ErrStream, (byte *)"\n", 1);
    DebugStack_Print(ErrStream, MORE);
    exit(1);
}

status Error_Init(MemCh *m){
    status r = READY;
    if(ErrorHandlers == NULL){
        ErrorHandlers = Lookup_Make(m, _TYPE_ZERO);
        r |= SUCCESS;
    }
    return r;
}
