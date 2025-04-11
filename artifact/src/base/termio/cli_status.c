#include <external.h>
#include <caneka.h>

status CliStatus_SetKey(MemCh *m, CliStatus *cli, Str *key, IntPair *pair){
    return Table_Set(cli->tbl, (Abstract *)key, (Abstract *)I64_Wrapped(m, *((util *)pair)));
}

status CliStatus_SetByKey(MemCh *m, CliStatus *cli, Str *key, Str *s){
    Single *sg = (Single *)Table_Get(cli->tbl, (Abstract *)key);
    if(sg != NULL){
        IntPair *pair = (IntPair *)&sg->val.value;
        if(cli->tbl->type.state & SUCCESS){
            StrVec *line = Span_Get(cli->lines, pair->a);
            Span_Set(line->p, pair->b, (Abstract *)s);
            return SUCCESS;
        }
    }
    return NOOP;
}

status CliStatus_Print(MemCh *m, CliStatus *cli){
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
            StrVec *line = (StrVec *)it.value;
            printf("\r\x1b[0K");
            if(line != NULL){
                StrVec_ToFd(line, 0);
            }
            write(1, "\n", 1);
        }
        return SUCCESS;
    }
    return NOOP;
}

status CliStatus_PrintFinish(MemCh *m, CliStatus *cli){
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

CliStatus *CliStatus_Make(MemCh *m, DoFunc render, Abstract *source){
    CliStatus *st = (CliStatus *)MemCh_Alloc(m, sizeof(CliStatus));
    st->type.of = TYPE_CLI_STATUS;
    st->lines = Span_Make(m);
    st->render = render;
    st->source = source;
    st->tbl = Span_Make(m);
    st->tbl->type.state |= FLAG_SPAN_TABLE;
    return st;
}
