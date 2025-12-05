#include <external.h>
#include <caneka.h>
#include <cnkcli_module.h>

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
    Str *secretKeyKey = K(m, "secret");
    Str *publicKeyKey = K(m, "public");
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
    Args_Add(cli, secretKeyKey, NULL, ARG_OPTIONAL,
        Sv(m, "Secret/private key to use."));
    Args_Add(cli, publicKeyKey, NULL, ARG_OPTIONAL,
        Sv(m, "Public key to use."));
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

    i32 code = 0;

    Str *config = CliArgs_Get(cli, configKey);
    NodeObj *configNode = NULL;
    if(config != NULL){
        Str *path = IoUtil_GetAbsPath(m, config);
        configNode = Config_FromPath(m, path);

        NodeObj *forever = Inst_ByPath(configNode,
            IoPath(m, "forever"), NULL, SPAN_OP_GET);
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
                struct timespec last;
                struct timespec taken;
                struct timespec threshold = {RESPAWN_THRESHOLD_SEC, 0};
                Time_Now(&last);
                args[2] = Time_ToStr(m, last);
                Out("^c.Spawn: logdir=$ cmd=@ time=$^0\n", args);
                ProcDets_Init(&pd);
                status r = SubProcess(m, cmd, &pd);
                if(r & ERROR){
                    codeSg->val.i = pd.code;
                    Out("^r.SubProcess returned error: logdir=$ cmd=@ time=$"
                        " return-code:$^0\n", args);
                }

                Time_Now(&taken);
                if(Time_Beyond(&last, &taken, &threshold)){
                    Out("^y.ReSpawn within throttle window, delaying:"
                        " logdir=$ cmd=@ time=$^0\n", args);
                    struct timespec delay = {1, 0};
                    struct timespec remaining;
                    Time_Delay(&delay, &remaining);
                }
            }
            if(r == READY){
                r |= NOOP;
            }
        }
    }

    if((r & (SUCCESS|ERROR|NOOP)) == 0){
        Str *inFileArg = CliArgs_Get(cli, inFileKey);
        Str *outFileArg = CliArgs_Get(cli, outFileKey);
        if(inFileArg != NULL || outFileArg != NULL){

            StrVec *headerPath = NULL;
            StrVec *footerPath = NULL;

            if(configNode != NULL){
                args[0] = configNode;
                args[1] = NULL;
                Out("^y.config @^0\n", args);

                NodeObj *html = Inst_ByPath(configNode, IoPath(m, "html"),
                    NULL, SPAN_OP_GET);
                if(html != NULL){
                    Table *atts = Seel_Get(html, K(m, "atts"));
                    headerPath = Table_Get(atts, K(m, "header"));
                    footerPath = Table_Get(atts, K(m, "footer"));
                }
            }

            StrVec *inPath = IoPath_From(m, inFileArg);
            StrVec *outPath = IoPath_From(m, outFileArg);
            StrVec *inExt = Path_Ext(m, inPath);
            StrVec *outExt = Path_Ext(m, outPath);

            args[0] = cli;
            args[1] = NULL;
            Out("^p.Args Found &^0\n", args);

            if(Equals(inExt, K(m, "fmt")) && Equals(outExt, K(m, "html"))){
                args[0] = inFileArg;
                args[1] = outFileArg;
                args[2] = NULL;
                Out("^c.Transpiling Pencil Fmt -> HTML: @/@^0\n", args);

                StrVec *content = File_ToVec(m, inFileArg);
                Cursor *curs = Cursor_Make(m, content); 
                Roebling *rbl = FormatFmt_Make(m, curs, NULL);
                r |= Roebling_Run(rbl);
                if(r & ERROR){
                    args[0] = inFileArg;
                    args[1] = NULL;
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Error preparing template for $", args);
                    code = 7;
                }else{
                    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED|BUFF_CLOBBER);
                    r |= File_Open(bf, outFileArg, O_CREAT|O_WRONLY|O_TRUNC);
                    if((r & ERROR) == 0 && headerPath != NULL){
                        Buff *_bf = Buff_Make(m, BUFF_UNBUFFERED);
                        r |= File_Open(_bf, StrVec_Str(m, headerPath), O_RDONLY);
                        r |= Buff_Pipe(bf, _bf);
                    }

                    if((r & ERROR) == 0){
                        r |= Fmt_ToHtml(bf, (Mess *)as(rbl->dest, TYPE_MESS));
                    }

                    if((r & ERROR) == 0 && footerPath != NULL){
                        Buff *_bf = Buff_Make(m, BUFF_UNBUFFERED);
                        r |= File_Open(_bf, StrVec_Str(m, footerPath), O_RDONLY);
                        r |= Buff_Pipe(bf, _bf);
                    }
                    r |= File_Close(bf);

                    if((r & (SUCCESS|ERROR)) != SUCCESS){
                        args[0] = bf;
                        args[1] = NULL;
                        Out("^r.Error of some kind @^0\n", args);
                        code = 7; 
                    }
                }
            }
        }
    }

    args[0] = cli;
    args[1] = NULL;
    Out("^p.Args @^0\n", args);

    CliArgs_Free(cli);

    DebugStack_Pop();
    return code;
}
