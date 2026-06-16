#include <external.h>
#include <caneka.h>

i32 main(int argc, char **argv){

    status r = READY;
    MemBook *cp = MemBook_Make(NULL);
    void *args[5];
    if(cp == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

#ifdef CNKOPT_EXT
    Caneka_Init(m);
#else
    Caneka_InitBase(m);
#endif

    Core_Direct(m, 1, 2);

    CliArgs *cli = CliArgs_Make(argc, argv);
    Debug_Push(m, cli);

    Str *help = K(m, "help");
    Str *noColor = K(m, "no-color");
    Str *url = K(m, "url");
    Str *dest = K(m, "dest");

    Args_Add(cli, help, NULL, ARG_OPTIONAL,
        Sv(m, "Show this help message."));
    Args_Add(cli, noColor, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, url, NULL, ARG_OPTIONAL,
        Sv(m, "Url to fetch."));
    Args_Add(cli, dest, NULL, ARG_OPTIONAL,
        Sv(m, "File to place binseg metadata"));

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, noColor)){
        Ansi_SetColor(FALSE);
    }

    void *ar[] = {cli, NULL};
    Out("^p.$^0\n", ar);

    return (r & ERROR) == 0 ? 0 : 1;
}
