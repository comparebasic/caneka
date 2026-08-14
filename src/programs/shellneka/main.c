#include <external.h>
#include <caneka.h>


i32 main(int argc, char **argv){

    status r = READY;
    MemBook *cp = MemBook_Make(NULL);
    if(cp == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    Core_Direct(m, 1, 2);

    CliArgs *cli = CliArgs_Make(argc, argv);
    Debug_Push(m, cli);

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *configKey = K(m, "config");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show this help message."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, configKey, NULL, ZERO, 
        Sv(m, "Path to config file."));

    if(CliArgs_Get(cli, noColorKey)){
        Ansi_SetColor(OutStream, FALSE);
    }

    CliArgs_Parse(cli);
    Node *config = Shellneka_Setup(m, cli);

    Shellneka_Serve(m, config);

    return (r & ERROR) == 0 ? 0 : 1;
}
