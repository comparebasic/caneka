#include <external.h>
#include <caneka.h>
#include <clineka_module.h>

Buff *BinSegCli_UnlogBuff(MemCh *m, Str *path){
    Buff *bf = Buff_Make(m, ZERO);
    if(Equals(path, Str_FromCstr(m, "-", ZERO))){
        struct pollfd pfd = {0, POLLIN, 0};
        if(poll(&pfd, 1, 1) == 0){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error stdin does not appear to be readable", NULL);
        }
        Buff_SetFd(bf, 0);
    }else{
        File_Open(bf, path, O_RDONLY);
    }
    if(bf->type.state & ERROR){

        void *args[2];
        args[0] = bf;
        args[1] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error opening UnlogBuff", args);
    }
    return bf;
}
