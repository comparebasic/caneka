#include <external.h>
#include <caneka.h> 

status OpenSsl_Error(Buff *bf){
    char _buff[256];
    unsigned long e = ERR_get_error();
    if(e != 0){
        char *openssl_err = ERR_error_string(e, _buff);
        void *args[] = {
            Str_CstrRef(bf->m, openssl_err),
            NULL
        };
        Fmt(bf, "  OpenSslError: ^rD.$^0", args);
    }
}
