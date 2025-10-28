#include <external.h>
#include <caneka.h>

status CliStatus_SetKey(MemCh *m, CliStatus *cli, Str *key, IntPair *pair){
    return Table_Set(cli->tbl, (Abstract *)key, (Abstract *)I64_Wrapped(m, *((util *)pair)));
}

status CliStatus_SetByKey(MemCh *m, CliStatus *cli, Str *key, Abstract *a){
    Single *sg = (Single *)Table_Get(cli->tbl, (Abstract *)key);
    if(sg != NULL){
        IntPair *pair = (IntPair *)&sg->val.value;
        if(cli->tbl->type.state & SUCCESS){
            FmtLine *line = (FmtLine *)Span_Get(cli->lines, pair->a);
            line->args[pair->b] = (Abstract *)a;
            return SUCCESS;
        }
    }
    return NOOP;
}

Abstract *CliStatus_GetByKey(MemCh *m, CliStatus *cli, Str *key){
    Single *sg = (Single *)Table_Get(cli->tbl, (Abstract *)key);
    if(sg != NULL){
        IntPair *pair = (IntPair *)&sg->val.value;
        if(cli->tbl->type.state & SUCCESS){
            FmtLine *line = (FmtLine *)Span_Get(cli->lines, pair->a);
            return line->args[pair->b];
        }
    }
    return NULL;
}

status CliStatus_Print(Stream *sm, CliStatus *cli){
    if((cli->render(sm->m, (Abstract *)cli) & NOOP) == 0){
        Iter it;
        Iter_Init(&it, cli->lines);
        int count = cli->lines->nvalues;
        if((cli->type.state & PROCESSING) != 0){
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(sm->m, cli->lines->nvalues),
                NULL
            };
            /*
            Fmt(sm, "\r\x1b[$A", args);
            */
        }
        if(count > 0){
            cli->type.state |= PROCESSING;
        }
        while((Iter_Next(&it) & END) == 0){
            /*
            Stream_Bytes(sm, (byte *)"\r\x1b[0K", 5);
            */
            if(it.value != NULL){
                FmtLine *line = (FmtLine *)it.value;
                Fmt(sm, line->fmt, line->args);
            }
            Stream_Bytes(sm, (byte *)"\n", 1);
        }
        return SUCCESS;
    }
    return NOOP;
}

status CliStatus_PrintFinish(Stream *sm, CliStatus *cli){
    Iter it;
    Iter_Init(&it, cli->lines);
    int count = cli->lines->nvalues;
    /*
    while((Iter_Next(&it) & END) == 0){
        Stream_Bytes(sm, (byte *)"\x1b[2K\r", 5);
    }
    */
    Stream_Bytes(sm, (byte *)"\n", 1);
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
