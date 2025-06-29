#include <external.h>
#include <caneka.h>
#include <tests.h>

static status test(MemCh *m){
    status r = READY;
    Tests_Init(m);
    r |= Test_Runner(m, "Caneka", Tests);
    return r;
}

i32 main(int argc, char **argv){
    MemBook *cp = MemBook_Make(NULL);
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    if(argc < 2){
        Out("fmt file.fmt\n", NULL);
        exit(1);
    }

    DebugStack_Push(NULL, 0);

    Str *path = File_GetAbsPath(m, Str_CstrRef(m, argv[1]));
    File *f = File_Make(m, path, NULL);
    f->sm->dest.curs->v->type.state |= DEBUG;
    File_Read(f, FILE_READ_MAX);

    Cursor *curs = File_GetCurs(f);

    Roebling *rbl = NULL;
    rbl = FormatFmt_Make(m, curs, NULL);
    rbl->type.state |= DEBUG;
    Roebling_Run(rbl);

    Stream *sm = Stream_MakeStrVec(m); 
    
    Fmt_ToHtml(sm, rbl->mess);
    if((rbl->mess->transp->type.state & SUCCESS) == 0){
        Abstract *args[] = {
            (Abstract *)path,
            NULL
        };
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Unable to parse fmt file", args);
    }

    Abstract *args[] = {
        (Abstract *)I32_Wrapped(ErrStream->m, f->sm->dest.curs->v->p->nvalues),
        (Abstract *)f,
        (Abstract *)rbl->mess->transp,
        NULL
    };

    Out("Count: $\n^c.File: &^0.\n^p.Mess: &^0.\n", args);

    DebugStack_Pop();
    return 0;
}
