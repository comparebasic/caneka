#include <external.h>
#include <caneka.h>

static boolean crashing = FALSE;

void *_Fatal(char *msg, cls t, i32 fd, char *func, char *file, int line){
#ifdef CLI    
    RawMode(FALSE);
#endif
    
    StrVec_FmtAdd(_debugM, NULL, 0, 
        "^r^Fatal Error: ^B_T^B - type(_T/_i4) _T:_T:_i4^0\n",
        msg, TYPE_CSTR, Type_ToChars(t), TYPE_CSTR, t, func, TYPE_CSTR, 
        file, TYPE_CSTR, line);
#ifdef OPENSSL
    char _buff[256];
    unsigned long e = ERR_get_error();
    if(e != 0){
        char *openssl_err = ERR_error_string(e, _buff);
        StrVec_FmtAdd(_debugM, NULL, 0"^rB^_T^0");
        StrVec_FmtAdd(_debugM, NULL, 0"  ^rB^_T^0", openssl_err, TYPE_CSTR);
    }
#endif
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

void LogError(char *msg, ...){
	va_list args;
    va_start(args, msg);
    if((GLOBAL_flags & NO_COLOR) == 0){
        printf("\x1b[31m");
    }
    vprintf(msg, args);
    if((GLOBAL_flags & NO_COLOR) == 0){
        printf("\x1b[0m");
    }
    printf("\n");
}

void ExitError(int code, char *msg, ...){
	va_list args;
    va_start(args, msg);
    if((GLOBAL_flags & NO_COLOR) == 0){
        printf("\x1b[31m");
    }
    vprintf(msg, args);
    if((GLOBAL_flags & NO_COLOR) == 0){
        printf("\x1b[0m");
    }
    printf("\n");
    exit(code);
}
