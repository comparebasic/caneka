#include <external.h>
#include <caneka.h>

ErrorMsg *ErrorMsg_Make(MemCh *m, char *func, char *file, int line, char *fmt, Abstract **args){
    ErrorMsg *msg = MemCh_AllocOf(m, sizeof(ErrorMsg), TYPE_ERROR_MSG);
    msg->type.of = TYPE_ERROR_MSG;

    i64 len = strlen(func);
    msg->lineInfo[0] = (Abstract *)Str_Ref(m, (byte *)func, len, len+1, STRING_COPY);
    len = strlen(file);
    msg->lineInfo[1] = (Abstract *)Str_Ref(m, (byte *)file, len, len+1, STRING_COPY);
    msg->lineInfo[2] = (Abstract *)I32_Wrapped(m, line);
    msg->lineInfo[3] = NULL;

    len = strlen(fmt);
    msg->fmt = Str_Ref(m, (byte *)fmt, len, len+1, STRING_COPY);

    i32 total = 0;
    while(args[total++] != NULL){}
    msg->args = Arr_Make(m, total);

    for(i32 i = 0; i < total; i++){
        msg->args[i] = args[i];
    }

    return msg;
}
