#include <external.h>
#include <caneka.h>
#include "../module.h"

Lookup *ErrorHandlers = NULL;
boolean _crashing = FALSE;
boolean _error = FALSE;

Abstract *ErrA = NULL;

static boolean _fuse = TRUE;
#ifdef SIGH_CATCH
static void sigH(i32 sig, siginfo_t *info, void *ptr){
    if(_fuse){
        _fuse = FALSE;
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Sig Seg Fault", NULL);
    }else{
        if(ErrStream->fd > 0){
            write(ErrStream->fd, "Double SigH\n", strlen("Double SigH\n"));
        }
    }
    exit(1);
}
#endif

static void sigQuit(i32 sig, siginfo_t *info, void *ptr){
    if(_fuse){
        _fuse = FALSE;
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, "Sig Quit", NULL);
    }else{
        if(ErrStream->fd > 0){
            write(ErrStream->fd, "Double SigH\n", strlen("Double SigH\n"));
        }
    }
    exit(1);
}

static void sigPipe(i32 sig, siginfo_t *info, void *ptr){
    if(_fuse){
        _fuse = FALSE;
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, "Sig Pipe", NULL);
    }else{
        if(ErrStream->fd > 0){
            write(ErrStream->fd, "Double SigH\n", strlen("Double SigH\n"));
        }
    }
    exit(1);
}


static void sigHup(i32 sig, siginfo_t *info, void *ptr){
    if(_fuse){
        _fuse = FALSE;
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, "Sig Hup", NULL);
    }else{
        if(ErrStream->fd > 0){
            write(ErrStream->fd, "Double SigH\n", strlen("Double SigH\n"));
        }
    }
    exit(1);
}

static void sigI(i32 sig, siginfo_t *info, void *ptr){
    if(_fuse){
        _fuse = FALSE;
        if(ErrA != NULL){
            void *args[] = {ErrA, NULL};
            Fmt(ErrStream, "^r.ErrA: &^0\n", args);
        }
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, "Sig Int", NULL);
    }else{
        if(ErrStream->fd > 0){
            write(ErrStream->fd, "Double SigH\n", strlen("Double SigH\n"));
        }
    }
    exit(1);
}

static struct sigaction _a;
static struct sigaction _b;
static struct sigaction _c;
static struct sigaction _d;
static struct sigaction _e;
static void setSigs(){
#ifdef SIGH_CATCH
    memset(&_a, 0, sizeof(struct sigaction));
    _a.sa_flags = SA_NODEFER;
    _a.sa_sigaction = sigH;
    sigaction(SIGSEGV, &_a, NULL);
#endif

    memset(&_b, 0, sizeof(struct sigaction));
    _b.sa_flags = SA_NODEFER;
    _b.sa_sigaction = sigI;
    sigaction(SIGINT, &_b, NULL);

    memset(&_c, 0, sizeof(struct sigaction));
    _c.sa_flags = SA_NODEFER;
    _c.sa_sigaction = sigQuit;
    sigaction(SIGTERM, &_c, NULL);

    memset(&_d, 0, sizeof(struct sigaction));
    _d.sa_flags = SA_NODEFER;
    _d.sa_sigaction = sigHup;
    sigaction(SIGHUP, &_d, NULL);

    memset(&_c, 0, sizeof(struct sigaction));
    _c.sa_flags = SA_NODEFER;
    _c.sa_sigaction = sigPipe;
    sigaction(SIGPIPE, &_c, NULL);
}

void Fatal(char *func, char *file, int line, char *fmt, void *args[]){
    if(_crashing){
        Fmt(ErrStream, "\n^r.Fatal called after crashing^0.\n", NULL);
        exit(9);
        return;
    }
#ifdef CLI 
    RawMode(FALSE);
#endif
    _crashing = TRUE;
#ifdef CLI 
    RawMode(FALSE);
#endif
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 0,
        .alloc = 0,
        .bytes = NULL
    };

    Fmt(ErrStream, "\n^r.FatalError: ", NULL);

    if(func != NULL){
        s.alloc = s.length = strlen(func);
        s.bytes = (byte *)func;
        ToS(ErrStream, &s, s.type.of, ZERO);
    }
    if(file != NULL){
        s.alloc = s.length = strlen(file);
        s.bytes = (byte *)file;
        Buff_AddBytes(ErrStream, (byte *)" ", 1);
        ToS(ErrStream, &s, s.type.of, ZERO);
    }
    if(line > 0){
        byte lineNo[MAX_BASE10+1];
        byte *b = lineNo;
        byte *end = b+MAX_BASE10;
        i64 length = Str_I64OnBytes(&b, end, line);
        s.alloc = s.length = length;
        s.bytes = b;
        Buff_AddBytes(ErrStream, (byte *)" line ", 6);
        ToS(ErrStream, &s, s.type.of, ZERO);
    }

    Buff_AddBytes(ErrStream, (byte *)" - ", 3);
    Fmt(ErrStream, fmt, args);

    /*
    if(errno != 0){
        void *args[] = {
            S(ErrStream->m, strerror(errno)),
            NULL
        };
        Fmt(ErrStream, "\n  IoError: ^D.$^d.", args);
    }
    */

#ifdef OPENSSL
    char _buff[256];
    unsigned long e = ERR_get_error();
    if(e != 0){
        char *openssl_err = ERR_error_string(e, _buff);
        void *args[] = {
            Str_CstrRef(ErrStream->m, openssl_err),
            NULL
        };
        Fmt(ErrStream, "  OpenSslError: ^rD.$^0", args);
    }
#endif

    Fmt(ErrStream, "^0.\n", NULL);
    DebugStack_Print(ErrStream, MORE);
    exit(13);
}

boolean IsZeroed(MemCh *m, byte *b, size_t sz, char *func, char *file, int line){
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
    Error(m, func, file, line, "Memory not Zeroed", NULL);
    return FALSE;
}

void Error(MemCh *m, char *func, char *file, int line, char *fmt, void *args[]){
    if(_error){
        _crashing = TRUE;
        Fatal(func, file, line, fmt, args);
        return;
    }
    _error = TRUE;
    status r = READY;
    Abstract *a = NULL;
    i16 g = 0;
    while(m != NULL && m->owner != NULL){
        Guard_Incr(ErrStream->m, &g, 6, FUNCNAME, FILENAME, LINENUMBER);
        a = m->owner;
        if(a->type.of == TYPE_MEMCTX){
            m = (MemCh *)a;
        }else{
            break;
        }
    }
    if(a != NULL){
        SourceFunc errFunc = (SourceFunc)Lookup_Get(ErrorHandlers, a->type.of);
        if(errFunc != NULL){
            ErrorMsg *msg = ErrorMsg_Make(m, func, file, line, fmt, args);
            r |= errFunc(m, a, msg);
        }
    }else{
        r |= ERROR;
    }

    if(r & ERROR){
        if(r & NOOP){
            Fmt(ErrStream, fmt, args);
            /*
            if(errno != 0){
                void *args[] = {
                    S(m, strerror(errno)),
                    NULL
                };
                Fmt(ErrStream, "\nIoError: ^D.$^d.", args);
            }
            */
        }else{
            Fatal(func, file, line, fmt, args);
        }
    }

    _error = FALSE;
    return;
}

status Error_Init(MemCh *m){
    status r = READY;
    setSigs();
    if(ErrorHandlers == NULL){
        ErrorHandlers = Lookup_Make(m, _TYPE_ZERO);
        r |= SUCCESS;
    }
    return r;
}
