#include <external.h>
#include <caneka.h>
#include <test_module.h>

#ifdef MEMTEST
    #define MEM_MAX_TESTS
#endif

i32 main(int argc, char **argv){
    printf("I\n");
    fflush(stdout);

    status r = READY;
    MemBook *cp = MemBook_Make(NULL);
    void *args[5];
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    Core_Direct(m, 1, 2);
    Inter_Init(m);
    DebugStack_Push(NULL, 0);

    CliArgs *cli = CliArgs_Make(argc, argv);

    Str *help = K(m, "help");
    Str *noColor = K(m, "no-color");
    Str *dist = K(m, "dist");

    Args_Add(cli, help, NULL, ARG_OPTIONAL,
        Sv(m, "Show this help message."));
    Args_Add(cli, noColor, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, dist, NULL, ARG_OPTIONAL,
        Sv(m, "Out to the dist directory instead of displaying results on the console."));
    Args_Add(cli, Str_FromCstr(m, "licence", STRING_COPY), NULL, ARG_OPTIONAL,
        Sv(m, "Show software licences."));
    Args_Add(cli, Str_FromCstr(m, "version", STRING_COPY), NULL, ARG_OPTIONAL,
        Sv(m, "Show the licences used in this application."));

    StrVec *name = StrVec_From(m, Str_FromCstr(m, argv[0], STRING_COPY));
    IoUtil_Annotate(m, name);
    Str *fname = IoUtil_FnameStr(m, name);

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, noColor)){
        Ansi_SetColor(FALSE);
    }

    Buff *bf = Buff_Make(m, ZERO);
    Str *path = Str_Make(m, STR_DEFAULT);
    if(CliArgs_Get(cli, dist) != NULL){
        Ansi_SetColor(FALSE);
        Str *dir = IoUtil_GetAbsPath(m, S(m, "dist/output"));
        Dir_CheckCreate(m, dir);

        args[0] = dir;
        args[1] = S(m, "/tests.fmt");
        args[2] = NULL;
        Str_AddChain(m, path, args);

        void *ar[] = {path, NULL};
        Out("^p.Outputting test results to @^\n", ar);

        bf->type.state |= BUFF_CLOBBER;
        File_Open(bf, path, O_WRONLY|O_CREAT|O_TRUNC);
        i32 outFd = 1;
        if(bf->fd > 0){
            outFd = bf->fd;
        }
        Core_Direct(m, outFd, 2);
    }

    printf("II\n");
    fflush(stdout);

    r |= Test_Runner(m, "Caneka base", BaseTests);
    r |= Test_Runner(m, "Caneka ext", ExtTests);
    r |= Test_Runner(m, "Caneka crypto", CryptoTests);
    r |= Test_Runner(m, "Caneka inter", InterTests);

    if(CliArgs_Get(cli, dist) != NULL){
        File_Close(bf);
        Core_Direct(m, 1, 2);
        args[0] = path;
        args[1] = NULL;
        Out("Tests run and output to $\n", args);
    }

    DebugStack_Pop();
    return 0;
}
