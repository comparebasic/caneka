#include <external.h>
#include <caneka.h>

status CliStatus_Print(MemCtx *m, CliStatus *cli){
    if((cli->render(m, (Abstract *)cli) & NOOP) == 0){
        Iter it;
        Iter_Init(&it, cli->lines);
        int count = 0;
        while((Iter_Next(&it) & END) == 0){
            String *line = (String *)Iter_Get(&it);
            if(line != NULL && line->length > 0){
                printf("\x1b[2K\r");
                ToStdOut(m, line, NULL); 
                count++;
            }
        }
        if(count > 0){
            printf("\r\x1b[%dA", count+1);
        }
        return SUCCESS;
    }
    return NOOP;
}

status CliStatus_PrintFinish(MemCtx *m, CliStatus *cli){
    for(int i = 0; i < cli->lines->nvalues; i++){
        printf("\r\x1b[2K\n");
    }
    fflush(stdout);
    return SUCCESS;
}

CliStatus *CliStatus_Make(MemCtx *m, DoFunc render, Abstract *source){
    CliStatus *st = (CliStatus *)MemCtx_Alloc(m, sizeof(CliStatus));
    st->type.of = TYPE_CLI_STATUS;
    st->lines = Span_Make(m, TYPE_SPAN);
    st->render = render;
    st->source = source;
    return st;
}
