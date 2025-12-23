#include <external.h>
#include <caneka.h>
#include <clineka_module.h>

i32 main(int argc, char **argv){
    void *args[16];
    status r = READY;
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
    Core_Direct(m, 1, 2);
    DebugStack_Push(NULL, 0);

    CliArgs *cli = CliArgs_Make(argc, argv);

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *inFileKey = K(m, "in");
    Str *outFileKey = K(m, "out");
    Str *configKey = K(m, "config");
    Str *licenceKey = K(m, "licence");
    Str *versionKey = K(m, "version");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL, Sv(m, "Show this help message."));

    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, inFileKey, NULL, ARG_OPTIONAL,
        Sv(m, "File input"));
    Args_Add(cli, outFileKey, NULL, ARG_OPTIONAL,
        Sv(m, "File output"));
    Args_Add(cli, configKey, NULL, ARG_OPTIONAL,
        Sv(m, "Derive actions to take from a *.config file."));
    Args_Add(cli, licenceKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show the licences used in this software"));
    Args_Add(cli, versionKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show the version of this software"));

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, noColorKey) != NULL){
        Ansi_SetColor(FALSE);
    }

    if(CliArgs_Get(cli, helpKey) != NULL){
        CharPtr_ToHelp(cli);
        return 1;
    }

    Str *config = CliArgs_Get(cli, configKey);
    NodeObj *configNode = NULL;
    if(config != NULL){
        Str *path = IoUtil_GetAbsPath(m, config);
        configNode = Config_FromPath(m, path);
    }

    CliArgs_Free(cli);

    DebugStack_Pop();
    return code;
}
