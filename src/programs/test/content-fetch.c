#include <external.h>
#include <caneka.h>
#include <test_module.h>

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

#ifdef CNKOPT_EXT
    Caneka_Init(m);
#else
    Caneka_InitBase(m);
#endif

    Core_Direct(m, 1, 2);
#ifdef CNKOPT_INTER
    Inter_Init(m);
#endif
    DebugStack_Push(NULL, 0);

    CliArgs *cli = CliArgs_Make(argc, argv);

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *sha256Key = K(m, "Sha256");
    Str *urlKey = K(m, "Sha256");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL, 
        Sv(m, "Show this help message."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, sha256Key, NULL, ARG_OPTIONAL,
        Sv(m, "Sha256 sum of content to match."));

    StrVec *name = StrVec_From(m, S(m, argv[0]));
    IoUtil_Annotate(m, name);
    Str *fname = IoUtil_FnameStr(m, name);

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, noColorKey)){
        Ansi_SetColor(FALSE);
    }

    DebugStack_Pop();
    return 0;
}
