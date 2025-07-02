#include <external.h>
#include <caneka.h>
#include <tests.h>

static status test(MemCh *m){
    status r = READY;
    Tests_Init(m);
    r |= Test_Runner(m, "Caneka", Tests);
    return r;
}

static status getHtmlDir(MemCh *m, Str *path){
    return NOOP;
}

static status dirFunc(MemCh *m, Str *path, Abstract *source){
    Str *newPath = Str_CloneAlloc(m, path, STR_DEFAULT);
    Str *replace = Str_CstrRef(m, "fmt");
    Span *backlog = Span_Make(m);
    Match *mt = Match_Make(m, PatChar_FromStr(m, replace), backlog);
    mt->type.state |= MATCH_SEARCH;
    Str *new = Str_CstrRef(m, "html");
    i32 pos = 0;
    Match_StrReplace(m, newPath, new, mt, &pos);

    Abstract *args[] = {
        (Abstract *)mt,
        (Abstract *)path,
        (Abstract *)newPath,
        NULL
    };
    Out("dirFunc: & @ making @\n", args);
    return Dir_CheckCreate(m, newPath);
}

static status fileFunc(MemCh *m, Str *path, Str *file, Abstract *source){
    Abstract *args[] = {
        (Abstract *)path,
        (Abstract *)file,
        NULL
    };
    Out("fileFunc: @ -> @\n", args);
    return SUCCESS;
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
    Table_Set(argResolve,  (Abstract *)Str_CstrRef(m, "module"), (Abstract *)True);
    Table *args = Table_Make(m);
    if(CharPtr_ToTbl(m, argResolve, argc, argv, args) & (ERROR|NOOP)){
        CharPtr_ToHelp(m, Str_CstrRef(m, "fmt"), argResolve, argc, argv);
        exit(1);
    }

    Str *module = (Str *)Table_Get(args, (Abstract *)Str_CstrRef(m, "module"));
    if(module == NULL){
        Error(m, (Abstract *)args, FUNCNAME, FILENAME, LINENUMBER, "module path not found", NULL);
    }
    Str *path = Str_Make(m, STR_DEFAULT);
    Str_AddCstr(path, "docs/fmt/");
    Str_Add(path, module->bytes, module->length);
    path = File_GetAbsPath(m, path);
    Abstract *_args[] = {
        (Abstract *)args,
        (Abstract *)path,
        NULL
    };

    Out("^y.Args @ and module path: @^0.\n", _args);

    Dir_Climb(m, path, dirFunc, fileFunc, NULL); 

    DebugStack_Pop();
    return 0;
}
