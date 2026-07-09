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

    Caneka_Init(m);
    Caneka_InitBase(m);
    Core_Direct(m, 1, 2);
    Inter_Init(m);

    CliArgs *cli = CliArgs_Make(argc, argv);
    Debug_Push(m, cli);

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *addKey = K(m, "add");
    Str *statusKey = K(m, "status");
    Str *nextkey = K(m, "next");
    Str *setMessagekey = K(m, "set-message");
    Str *syncKey = K(m, "sync");

    Args_Add(cli, helpKey, NULL, ARG_MULTIPLE,
        Sv(m, "Show this help message."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));

    Table *cmdChoices = Table_Make(m);
    Table_Set(cmdChoices, addKey, 
        Sv(m, "Add a all or a specific path of changes."));
    Table_Set(cmdChoices, statusKey, 
        Sv(m, "Show current status or a historical log of status."));
    Table_Set(cmdChoices, nextKey, Sv(m, "Set a fresh changeset."));
    Table_Set(cli, setMessageKey, 
        Sv(m, "Set the message of the current changeset."));
    Table_Set(cli, syncKey, 
        Sv(m, "Synchronize changes with a remote host."));

    Args_Add(cli, Str_FromCstr(m, "cmd", STRING_COPY), cmdChoices, ARG_CHOICE,
        Sv(m, "Command to run for repository."));

    Args_Add(cli, Str_FromCstr(m, "licence", STRING_COPY), NULL, ARG_OPTIONAL,
        Sv(m, "Show software licences."));
    Args_Add(cli, Str_FromCstr(m, "version", STRING_COPY), NULL, ARG_OPTIONAL,
        Sv(m, "Show the licences used in this application."));

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, noColor)){
        Ansi_SetColor(OutStream, FALSE);
    }

    void *ar[] = {cli, NULL};
    Out("^p.@^0\n", ar);

    Return(m, 0);
}
