#include <external.h>
#include <caneka.h>
#include <tests.h>

i32 main(int argc, char **argv){
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            char *arg = argv[i];
            if(strncmp(arg, "no-color", strlen("no-color")) == 0){
                GLOBAL_flags |= NO_COLOR;
            }
            if(strncmp(arg, "html", strlen("html")) == 0){
                GLOBAL_flags |= HTML_OUTPUT;
            }
        }
    }

    MemBook *cp = MemBook_Make(NULL);
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    Stream_Init(m, 1, 2);
    Inter_Init(m);
    DebugStack_Push(NULL, 0);

    Str *helpKey = S(m, "help");
    Str *noColorKey = S(m, "no-color");
    Str *sha256Key = S(m, "Sha256");
    Str *urlKey = S(m, "Sha256");

    Table *resolveArgs = Table_Make(m);
    Args_Add(resolveArgs, helpKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, noColorKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, sha256Key, ARG_OPTIONAL);
    Args_Add(resolveArgs, sha256Key, ARG_OPTIONAL);

    StrVec *name = StrVec_From(m, S(m, argv[0]));
    IoUtil_Annotate(m, name);
    Str *fname = IoUtil_FnameStr(m, name);

    Table *cliArgs = Table_Make(m);
    CharPtr_ToTbl(m, resolveArgs, argc, argv, cliArgs);
    if(Table_GetHashed(cliArgs, help) != NULL){
        CharPtr_ToHelp(m, fname, resolveArgs, argc, argv);
        return 1;
    }

    if(Table_GetHashed(cliArgs, noColor) != NULL){
        Ansi_SetColor(FALSE);
    }



    DebugStack_Pop();
    return 0;
}
