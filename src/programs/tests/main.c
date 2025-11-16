#include <external.h>
#include <caneka.h>
#include <tests.h>
#include <tests_list.c>

#ifdef MEMTEST
    #define MEM_MAX_TESTS
#endif

static status test(MemCh *m){
    status r = READY;
    Tests_Init(m);
    r |= Test_Runner(m, "Caneka", Tests);
    return r;
}

status Tests_Init(MemCh *m){
   status r = READY;
   r |= Make109Strs(m);
   return r;
}


i32 main(int argc, char **argv){
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

    Str *help = K(m, "help");
    Str *noColor = K(m, "no-color");
    Str *dist = K(m, "dist");

    Table *resolveArgs = Table_Make(m);
    Args_Add(resolveArgs, help, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, noColor, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, dist, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs,
        Str_FromCstr(m, "licence", STRING_COPY), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs,
        Str_FromCstr(m, "version", STRING_COPY), NULL, ARG_OPTIONAL);

    StrVec *name = StrVec_From(m, Str_FromCstr(m, argv[0], STRING_COPY));
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

    Buff *bf = Buff_Make(m, ZERO);
    Str *path = Str_Make(m, STR_DEFAULT);
    if(Table_GetHashed(cliArgs, dist) != NULL){
        Ansi_SetColor(FALSE);
        Str *dir = IoUtil_GetAbsPath(m, S(m, "dist/output"));
        Dir_CheckCreate(m, dir);

        args[0] = dir;
        args[1] = S(m, "/tests.fmt");
        args[2] = NULL;
        Str_AddChain(m, path, args);

        bf->type.state |= BUFF_CLOBBER;
        File_Open(bf, path, O_WRONLY|O_CREAT|O_TRUNC);
        i32 outFd = 1;
        if(bf->fd > 0){
            outFd = bf->fd;
        }
        Core_Direct(m, outFd, 2);
    }

    test(m);

    if(Table_GetHashed(cliArgs, dist) != NULL){
        File_Close(bf);
        Core_Direct(m, 1, 2);
        args[0] = path;
        args[1] = NULL;
        Out("Tests run and output to $\n", args);
    }

    DebugStack_Pop();
    return 0;
}
