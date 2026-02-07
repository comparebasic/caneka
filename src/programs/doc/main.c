#include <external.h>
#include <caneka.h>
#include <doc_module.h>

i32 main(int argc, char **argv){
    status r = READY;
    MemBook *cp = MemBook_Make(NULL);
    i32 code = 0;
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
            "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
            "MemCh created successfully", NULL);
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

    Str *configPath = CliArgs_Get(cli, configKey);
    NodeObj *config = NULL;
    if(configPath == NULL){
        Out("^y.No config specified, exiting.^0\n", NULL);
        code = 1;
    }else{
        Str *path = IoUtil_GetAbsPath(m, configPath);
        config = Config_FromPath(m, path);

        StrVec *root = NULL;
        StrVec *fmtRoot = NULL;
        StrVec *cRoot = NULL;
        StrVec *outDir = NULL;
        WwwPage *srcPage = NULL;
        WwwPage *fmtPage = NULL;
        WwwNav *nav = NULL;

        NodeObj *out = Inst_ByPath(config,
            Sv(m, "out"), NULL, SPAN_OP_GET, NULL);

        outDir = Inst_Att(out, K(m, "dir"));
        NodeObj *pageObj = Inst_ByPath(out, Sv(m, "page"), NULL, SPAN_OP_GET, NULL);
        WwwPage *page = Inst_Make(m, TYPE_WWW_PAGE);
        Seel_Set(page, K(m, "name"), Sv(m, "docPage"));

        /* header */
        StrVec *headerPath = IoUtil_AbsVec(m, Inst_Att(pageObj, K(m, "header")));

        Gen *headerGen = Gen_FromPath(m, headerPath, NULL);
        Gen_Setup(m, headerGen, NULL);
        /* footer */
        StrVec *footerPath = IoUtil_AbsVec(m, Inst_Att(pageObj, K(m, "footer")));
        Gen *footerGen = Gen_FromPath(m, footerPath, NULL);
        Gen_Setup(m, footerGen, NULL);
        /* nav */
        StrVec *navPath = IoUtil_AbsVec(m, Inst_Att(pageObj, K(m, "nav")));
        Gen *navGen = Gen_FromPath(m, navPath, NULL);
        Gen_Setup(m, navGen, NULL);

        NodeObj *in = Inst_ByPath(config, Sv(m, "in"), NULL, SPAN_OP_GET, NULL);

        Table *dirTbl = Table_Make(m);
        Inst_ChAttsAdd(in, K(m, "dir"), dirTbl);
        Span *files = Span_Make(m);
        Doc_Gather(dirTbl, files);

        nav = Inst_Make(m, TYPE_WWW_NAV);
        Inst_SetAtt(nav, K(m, "coords"), Table_Make(m));
        Doc_GenNav(config, files, nav);

        /*
        void *args[] = {
            in,
            outDir,
            page,
            footerGen,
            headerGen,
            NULL
        };
        Out("^y.in @\nout @\n page @^0\nheader @\nfooter \n",
            args);
        */

        Iter it;
        for(Iter2d_InstInit(m, nav, &it); (it.type.state & END) == 0;
                Iter2d_InstNext(&it)){
            Abstract *a = Iter2d_Get(&it);
            if(a != NULL){
                if(a->type.of == TYPE_WWW_NAV){
                    StrVec *name = Seel_Get((Inst *)a, K(m, "name"));
                    if(!Empty(name) && !Equals(name, S(m, "README"))
                             && !Equals(name, S(m, "Inc"))){

                        Inst *page = Inst_Make(m, TYPE_WWW_PAGE);

                        Table *coordTbl = Inst_Att(nav, K(m, "coords"));
                        Iter *navIt = Iter_Make(m, NULL);
                        Span *crd = Table_Get(coordTbl, name);
                        NestSel_Init(navIt, nav, crd);
                        Seel_Set(page, S(m, "nav"), navIt); 

                        StrVec *dest = NULL;
                        Doc_FileOut(page, (WwwNav *)a, dest);
                        exit(1);
                    }
                }else{
                    void *ar[] = {
                        a,
                        NULL
                    };
                    Out("^y.? @^0\n", ar);
                }
            }
        }
    }

    CliArgs_Free(cli);

    DebugStack_Pop();
    return code;
}
