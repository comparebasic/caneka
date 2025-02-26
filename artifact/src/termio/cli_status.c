#include <external.h>
#include <caneka.h>

status CliStatus_Print(MemCtx *m, CliStatus *cli){
    if((cli->render(m, (Abstract *)cli) & NOOP) == 0){
        Iter it;
        Iter_Init(&it, cli->lines);
        int count = cli->lines->nvalues;
        if((cli->type.state & PROCESSING) != 0){
            printf("\r\x1b[%dA", count);
            fflush(stdout);
        }
        if(count > 0){
            cli->type.state |= PROCESSING;
        }
        while((Iter_Next(&it) & END) == 0){
            StrVec *line = (StrVec *)Iter_Get(&it);
            printf("\r\x1b[0K");
            fflush(stdout);
            if(line != NULL){
                StrVec_ToFd(line, 1);
            }
            write(1, "\n", 1);
        }
        return SUCCESS;
    }
    return NOOP;
}

status CliStatus_PrintFinish(MemCtx *m, CliStatus *cli){
    Iter it;
    Iter_Init(&it, cli->lines);
    int count = cli->lines->nvalues;
    if((cli->type.state & PROCESSING) != 0){
        printf("\r\x1b[%dA", count);
        fflush(stdout);
    }
    while((Iter_Next(&it) & END) == 0){
        printf("\r\x1b[0K");
        fflush(stdout);
        write(1, "\n", 1);
    }
    cli->type.state &= ~PROCESSING;
    return SUCCESS;
}

status CliStatus_SetDims(CliStatus *cli, i32 cols, i32 rows){
    if(cols == 0 && rows == 0){
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);

        cli->cols = w.ws_col;
        cli->rows = w.ws_row;
    }else{
        cli->cols = cols;
        cli->rows = rows;
    }
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
