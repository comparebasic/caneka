#include <external.h>
#include "base_module.h"

static Str **cliLabels = NULL;

status CliArgs_Print(Buff *bf, void *a, cls type, word flags){
    MemCh *m = bf->m;
    CliArgs *cli = (CliArgs *)Ifc(bf->m, a, TYPE_CLI_ARGS);
    Span *p = Span_Make(m);
    Iter it;
    Iter_Init(&it, p);
    i32 i = 0;
    while(cli->argv != NULL && cli->argv[i] != NULL){
        Span_Add(p, S(m, cli->argv[i++]));
    }
    void *args[] = {
        Type_StateVec(m, cli->type.of, cli->type.state),
        I32_Wrapped(m, cli->argc),
        p,
        cli->args,
        NULL
    };
    return Fmt(bf, "CliArgs<@ args=$/@ resolved to @>", args);
}

status Termio_ToSInit(MemCh *m){
    status r = READY;
    r |= Lookup_Add(m, ToStreamLookup, TYPE_CLI_ARGS, (void *)CliArgs_Print);
    return r;
}
