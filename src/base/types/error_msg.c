#include <external.h>
#include <caneka.h>

ErrorMsg *ErrorMsg_Make(MemCh *m, char *func, char *file, int line, char *fmt, void **args){
    ErrorMsg *msg = MemCh_AllocOf(m, sizeof(ErrorMsg), TYPE_ERROR_MSG);
    msg->type.of = TYPE_ERROR_MSG;

    msg->lineInfo[0] = Str_FromCstr(m, func, STRING_COPY);
    msg->lineInfo[1] = Str_FromCstr(m, file, STRING_COPY);
    msg->lineInfo[2] = I32_Wrapped(m, line);
    msg->lineInfo[3] = NULL;

    msg->fmt = Str_FromCstr(m, fmt, STRING_COPY);

    i32 total = 0;
    while(args[total++] != NULL){}
    msg->args = Arr_Make(m, total);

    for(i32 i = 0; i < total; i++){
        msg->args[i] = args[i];
    }

    return msg;
}
