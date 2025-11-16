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
    Core_Direct(m, 1, 2);
    DebugStack_Push(NULL, 0);

    Table *resolveArgs = Table_Make(m);
    Args_Add(resolveArgs,
        S(m, "help"), NULL, ARG_OPTIONAL);

    Span *actions = Span_Make(m);
        Span_Add(actions, S(m, "pencil"));
        Span_Add(actions, S(m, "binseg"));
        Span_Add(actions, S(m, "templ"));
    Args_Add(resolveArgs, S(m, "action"), actions, (ARG_CHOICE|ARG_DEFAULT));

    Args_Add(resolveArgs, S(m, "out-file"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "in-file"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "no-color"), NULL, ARG_OPTIONAL);

    Args_Add(resolveArgs, S(m, "config"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "reversed"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "secret-key"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "public-key"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "licence"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "version"), NULL, ARG_OPTIONAL);
    Span *transOptions = Span_Make(m); Span_Add(transOptions, S(m, "terminal"));
        Span_Add(transOptions, S(m, "json"));
        Span_Add(transOptions, S(m, "pencil"));
        Span_Add(transOptions, S(m, "binseg"));
        Span_Add(transOptions, S(m, "binseg-r"));
    Args_Add(resolveArgs, S(m, "trans"), transOptions, (ARG_CHOICE|ARG_DEFAULT));

    StrVec *name = StrVec_From(m, S(m, argv[0]));
    IoUtil_Annotate(m, name);
    Str *fname = IoUtil_FnameStr(m, name);

    Table *cliArgs = Table_Make(m);
    CharPtr_ToTbl(m, resolveArgs, argc, argv, cliArgs);
    if(Table_GetHashed(cliArgs, S(m, "help")) != NULL){
        CharPtr_ToHelp(m, fname, resolveArgs, argc, argv);
        return 1;
    }

    if(Table_GetHashed(cliArgs, S(m, "no-color")) != NULL){
        Ansi_SetColor(FALSE);
    }

    i32 code = 0;

    Str *config = Table_Get(cliArgs, K(m, "config"));
    if(config != NULL){
        Str *path = IoUtil_GetAbsPath(m, config);
        NodeObj *config = Config_FromPath(m, path);

        NodeObj *forever = Inst_ByPath(config, IoPath(m, "forever"), NULL, SPAN_OP_GET);
        if(forever != NULL){
            Table *atts = Seel_Get(forever, K(m, "atts"));
            StrVec *logdir = Table_Get(atts, K(m, "logdir"));

            StrVec *cmdV = Table_Get(atts, K(m, "cmd"));
            Path_SpaceAnnotate(m, cmdV);
            Span *cmd = IoUtil_AbsCmdArr(m, cmdV);

            Single *codeSg = I32_Wrapped(m, 0);
            ProcDets pd;
            args[0] = logdir;
            args[1] = cmd;
            args[2] = MicroTime_ToStr(m, MicroTime_Now());
            args[3] = codeSg;
            args[4] = NULL;
            Out("^c.Forever: logdir=$ cmd=@ time=$^0\n", args);

            boolean run = TRUE;
            while(run){
                microTime last = MicroTime_Now();
                args[2] = MicroTime_ToStr(m, last);
                Out("^c.Spawn: logdir=$ cmd=@ time=$^0\n", args);
                ProcDets_Init(&pd);
                status r = SubProcess(m, cmd, &pd);
                if(r & ERROR){
                    codeSg->val.i = pd.code;
                    Out("^r.SubProcess returned error: logdir=$ cmd=@ time=$ return-code:$^0\n", args);
                }

                if((last - MicroTime_Now()) < (TIME_SEC / 2)){
                    Out("^y.ReSpawn within throttle window, delaying:"
                        " logdir=$ cmd=@ time=$^0\n", args);
                    microTime remaining;
                    Time_Delay(TIME_SEC, &remaining);
                }
            }
        }
    }

    DebugStack_Pop();
    return code;
}
