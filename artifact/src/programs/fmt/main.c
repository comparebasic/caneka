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

    DebugStack_Push(NULL, 0);

    Table *argResolve = Table_Make(m);
    Single *True = I32_Wrapped(m, TRUE);
    Table_Set(argResolve, (Abstract *)Str_CstrRef(m, "debug"), (Abstract *)True);
    Table_Set(argResolve,  (Abstract *)Str_CstrRef(m, "out"), (Abstract *)True);
    Table_Set(argResolve,  (Abstract *)Str_CstrRef(m, "in"), (Abstract *)True);
    Table *args = Table_Make(m);
    if(CharPtr_ToTbl(m, argResolve, argc, argv, args) & (ERROR|NOOP)){
        CharPtr_ToHelp(m, Str_CstrRef(m, "fmt"), argResolve, argc, argv);
        exit(1);
    }

    Str *fname = (Str *)Table_Get(args, (Abstract *)Str_CstrRef(m, "in"));
    Str *path = IoUil_GetAbsPath(m, fname);
    File *f = File_Make(m, path, NULL);
    File_Read(f, FILE_READ_MAX);

    Cursor *curs = File_GetCurs(f);

    Roebling *rbl = NULL;
    rbl = FormatFmt_Make(m, curs, NULL);

    Abstract *debug = Table_Get(args, (Abstract *)Str_CstrRef(m, "debug"));
    if(debug->type.of == TYPE_WRAPPED_PTR || 
            Equals((Abstract *)debug,  (Abstract *)Str_CstrRef(m, "roebling"))){
        rbl->type.state |= DEBUG;
    }

    if(debug->type.of == TYPE_WRAPPED_PTR || 
            Equals((Abstract *)debug,  (Abstract *)Str_CstrRef(m, "mess"))){
        rbl->mess->type.state |= DEBUG;
    }

    Roebling_Run(rbl);

    Stream *sm = OutStream; 
    
    rbl->mess->type.state |= (rbl->type.state & DEBUG);
    Fmt_ToHtml(sm, rbl->mess);
    if((rbl->mess->transp->type.state & SUCCESS) == 0){
        Abstract *args[] = {
            (Abstract *)path,
            NULL
        };
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Unable to parse fmt file", args);
    }

    DebugStack_Pop();
    return 0;
}
