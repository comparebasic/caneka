#include <external.h>
#include <caneka.h>
#include <cnkcli.h>

i32 main(int argc, char **argv){
    void *args[5];
    MemBook *cp = MemBook_Make(NULL);
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    Inter_Init(m);
    DebugStack_Push(NULL, 0);

    Table *resolveArgs = Table_Make(m);
    Args_Add(resolveArgs,
        S(m, "help"), NULL, ARG_OPTIONAL);

    Span *actions = Span_Make(m);
        Span_Add(actions, S(m, "pencil"));
        Span_Add(actions, S(m, "binseg"));
        Span_Add(actions, S(m, "templ"));
    Args_Add(resolveArgs, S(m, "action"), actions, (ARG_CHOICE|ARG_DEFAULT));

    Args_Add(resolveArgs, S(m, "out-file"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "in-file"), NULL, ARG_OPTIONAL);

    Args_Add(resolveArgs, S(m, "config"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "reversed"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "secret-key"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "public-key"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "licence"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "version"), NULL, ARG_OPTIONAL);
    Span *transOptions = Span_Make(m); Span_Add(transOptions, S(m, "terminal"));
        Span_Add(transOptions, S(m, "json"));
        Span_Add(transOptions, S(m, "pencil"));
        Span_Add(transOptions, S(m, "binseg"));
        Span_Add(transOptions, S(m, "binseg-r"));
    Args_Add(resolveArgs, S(m, "trans"), transOptions, (ARG_CHOICE|ARG_DEFAULT));

    StrVec *name = StrVec_From(m, S(m, argv[0]));
    IoUtil_Annotate(m, name);
    Str *fname = IoUtil_FnameStr(m, name);

    Table *cliArgs = Table_Make(m);
    CharPtr_ToTbl(m, resolveArgs, argc, argv, cliArgs);
    if(Table_GetHashed(cliArgs, S(m, "help")) != NULL){
        CharPtr_ToHelp(m, fname, resolveArgs, argc, argv);
        return 1;
    }

    Str *config = Table_Get(cliArgs, K(m, "config"));
    if(config != NULL){
        StrVec *path = IoAbsPath(m, config);
        NodeObj *config = Config_FromPath(m, path);
        args[0] = config;
        args[1] = NULL;
        Out("^p.config @^0\n");
    }

    DebugStack_Pop();
    return 0;
}
