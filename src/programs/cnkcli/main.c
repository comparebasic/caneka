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

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *inFileKey = K(m, "in");
    Str *outFileKey = K(m, "out");
    Str *configKey = K(m, "config");
    Str *secretKeyKey = K(m, "secret");
    Str *publicKeyKey = K(m, "public");
    Str *licenceKey = K(m, "licence");
    Str *versionKey = K(m, "version");

    Table *resolveArgs = Table_Make(m);
    Args_Add(resolveArgs, helpKey, NULL, ARG_OPTIONAL);

    Args_Add(resolveArgs, noColorKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, inFileKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, outFileKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, configKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, configKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, secretKeyKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, publicKeyKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, licenceKey, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, versionKey, NULL, ARG_OPTIONAL);

    Str *fname = S(m, argv[0]);

    Table *cliArgs = Table_Make(m);
    CharPtr_ToTbl(m, resolveArgs, argc, argv, cliArgs);
    if(Table_GetHashed(cliArgs, helpKey) != NULL){
        CharPtr_ToHelp(m, fname, resolveArgs, argc, argv);
        return 1;
    }

    if(Table_GetHashed(cliArgs, noColorKey) != NULL){
        Ansi_SetColor(FALSE);
    }

    i32 code = 0;

    Str *config = Table_Get(cliArgs, configKey);
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
    }else{
        Str *inFileArg = Table_Get(cliArgs, inFileKey);
        Str *outFileArg = Table_Get(cliArgs, outFileKey);
        if(inFileKey != NULL || outFileKey != NULL){

            StrVec *inPath = IoUtil_AbsVec(m, StrVec_From(m, inFileKey));
            StrVec *outPath = IoUtil_AbsVec(m, StrVec_From(m, outFileKey));
            StrVec *inExt = Path_Ext(m, inPath);
            StrVec *outExt = Path_Ext(m, outPath);

            args[0] = cliArgs;
            args[1] = inExt;
            args[2] = outExt;
            args[3] = NULL;
            Out("^p.Args Found @\n  ^y.in:@ out:@^0\n", args);
        }
    }

    DebugStack_Pop();
    return code;
}
