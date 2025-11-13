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
        Str_FromCstr(m, "help", STRING_COPY), NULL, ARG_OPTIONAL);

    Span *actions = Span_Make(m);
        Span_Add(actions, Str_FromCstr(m, "pencil", STRING_COPY));
        Span_Add(actions, Str_FromCstr(m, "binseg", STRING_COPY));
        Span_Add(actions, Str_FromCstr(m, "templ", STRING_COPY));
    Args_Add(resolveArgs, Str_FromCstr(m, "action", STRING_COPY),
        actions, (ARG_CHOICE|ARG_DEFAULT));

    Args_Add(resolveArgs,
        Str_FromCstr(m, "out-file", STRING_COPY), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs,
        Str_FromCstr(m, "in-file", STRING_COPY), NULL, ARG_OPTIONAL);

    Args_Add(resolveArgs,
        Str_FromCstr(m, "reversed", STRING_COPY), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs,
        Str_FromCstr(m, "secret-key", STRING_COPY), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs,
        Str_FromCstr(m, "public-key", STRING_COPY), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs,
        Str_FromCstr(m, "licence", STRING_COPY), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs,
        Str_FromCstr(m, "version", STRING_COPY), NULL, ARG_OPTIONAL);
    Span *transOptions = Span_Make(m);
        Span_Add(transOptions, Str_FromCstr(m, "terminal", STRING_COPY));
        Span_Add(transOptions, Str_FromCstr(m, "json", STRING_COPY));
        Span_Add(transOptions, Str_FromCstr(m, "pencil", STRING_COPY));
        Span_Add(transOptions, Str_FromCstr(m, "binseg", STRING_COPY));
        Span_Add(transOptions, Str_FromCstr(m, "binseg-r", STRING_COPY));
    Args_Add(resolveArgs,
        Str_FromCstr(m, "trans", STRING_COPY),
        transOptions,
        (ARG_CHOICE|ARG_DEFAULT));

    StrVec *name = StrVec_From(m, Str_FromCstr(m, argv[0], STRING_COPY));
    IoUtil_Annotate(m, name);
    Str *fname = IoUtil_FnameStr(m, name);

    Table *cliArgs = Table_Make(m);
    CharPtr_ToTbl(m, resolveArgs, argc, argv, cliArgs);
    if(Table_GetHashed(cliArgs, Str_FromCstr(m, "help", ZERO)) != NULL){
        CharPtr_ToHelp(m, fname, resolveArgs, argc, argv);
        return 1;
    }

    args[0] = cliArgs;
    args[1] = NULL;
    Out("^p.args recieved @^0\n", args);

    DebugStack_Pop();
    return 0;
}
