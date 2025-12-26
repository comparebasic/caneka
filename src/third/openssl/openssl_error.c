#include <external.h>
#include <caneka.h> 

status OpenSsl_Error(Buff *bf){
    char _buff[512];
    unsigned long e = ERR_get_error();
    if(e != 0){
        ERR_error_string_n(e, _buff, 512);
        void *args[] = {
            Str_CstrRef(bf->m, _buff),
            NULL
        };
        Fmt(bf, "  OpenSslError: ^rD.$^0", args);
    }
}
