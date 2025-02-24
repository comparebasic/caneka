#include <external.h>
#include <caneka.h>

status CliStatus_Print(MemCtx *m, CliStatus *cli){
    if((cli->render(m, (Abstract *)cli) & NOOP) == 0){
        Iter it;
        Iter_Init(&it, cli->lines);
        int count = cli->lines->nvalues;
        if((cli->type.state & PROCESSING) != 0){
            printf("\r\x1b[%dA", count);
        }
        if(count > 0){
            cli->type.state |= PROCESSING;
        }
        while((Iter_Next(&it) & END) == 0){
            String *line = (String *)Iter_Get(&it);
            printf("\r\x1b[0K");
            if(line != NULL && line->length > 0){
                ToStdOut(m, line, NULL); 
            }else{
                printf("\n");
            }
        }
        return SUCCESS;
    }
    return NOOP;
}

status CliStatus_PrintFinish(MemCtx *m, CliStatus *cli){
    cli->type.state &= ~PROCESSING;
    return SUCCESS;
}

CliStatus *CliStatus_Make(MemCtx *m, DoFunc render, Abstract *source){
    CliStatus *st = (CliStatus *)MemCtx_Alloc(m, sizeof(CliStatus));
    st->type.of = TYPE_CLI_STATUS;
    st->lines = Span_Make(m, TYPE_SPAN);
    st->buffs = Span_Make(m, TYPE_SPAN);
    st->render = render;
    st->source = source;
    return st;
}
