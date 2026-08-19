#include <external.h>
#include <caneka.h>
#include <doc_module.h>

i32 main(int argc, char **argv){
    status r = READY;
    MemBook *cp = MemBook_Make(NULL);
    i32 code = 0;
    if(cp == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, 
            "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, 
            "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    Core_Direct(m, 1, 2);

    CliArgs *cli = CliArgs_Make(argc, argv);
    Debug_Push(m, cli);

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *dirKey = K(m, "dir");
    Str *outDirKey = K(m, "out-dir");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL, Sv(m, "Show this help message."));

    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, dirKey, S(m, "src"), ARG_DEFAULT, Sv(m, "Source directory"));
    Args_Add(cli, outDirKey, S(m, "dist/doc") , ARG_DEFAULT, Sv(m, "Source directory"));

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, noColorKey) != NULL){
        Ansi_SetColor(OutStream, FALSE);
    }

    if(CliArgs_Get(cli, helpKey) != NULL){
        CharPtr_ToHelp(cli);
        return 1;
    }

    Str *dirPath = CliArgs_Get(cli, dirKey);
    Str *outDirPath = CliArgs_Get(cli, outDirKey);

    StrVec *dir = IoUtil_GetAbsVec(m, dirPath);
    StrVec *outDir = IoUtil_GetAbsVec(m, outDirPath);

    Node *ctx = Inst_Make(m, TYPE_NODE);
    Inst_SetChild(ctx, S(m, "dir"), dir);
    Inst_SetChild(ctx, S(m, "outDir"), dir);
    Doc_GenNav(ctx);

    CliArgs_Free(cli);

    return code;
}
