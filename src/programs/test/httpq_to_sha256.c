#include <external.h>
#include <caneka.h>

static char *getReq = ""
    "GET / HTTP/1.1\r\n"
    "Host: firecrow.com\r\n"
    "User-Agent: CanekaCli\r\n"
    "\r\n";

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

#ifdef CNKOPT_EXT
    Caneka_Init(m);
#else
    Caneka_InitBase(m);
#endif

    Core_Direct(m, 1, 2);

    CliArgs *cli = CliArgs_Make(argc, argv);
    Debug_Push(m, cli);

    Str *help = K(m, "help");
    Str *noColor = K(m, "no-color");
    Str *url = K(m, "url");
    Str *dest = K(m, "dest");

    Args_Add(cli, help, NULL, ARG_OPTIONAL,
        Sv(m, "Show this help message."));
    Args_Add(cli, noColor, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, url, NULL, ARG_OPTIONAL,
        Sv(m, "Url to fetch."));
    Args_Add(cli, dest, NULL, ARG_OPTIONAL,
        Sv(m, "File to place binseg metadata"));

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, noColor)){
        Ansi_SetColor(FALSE);
    }

    args[0] = cli;
    args[1] = NULL;
    Out("^y.$^0\n", args);

    Str *text = S(m, getReq);

    Str *name = S(m, "firecrow.com");
    HostEnt *h = HostEnt_FromName(m, name);

    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
    Conn_InetConnect(bf, h, 80);
    Buff_Add(bf, text); 

    HttpReq *req = (HttpReq *)HttpReq_MakeResp(m, bf->fd);
    while((req->type.state & (SUCCESS|ERROR)) == 0){
        HttpReq_ReadToRbl(req);
    }
    HttpReq_Close(req);

    args[0] = req;
    args[1] = NULL;
    Out("^p.Req: &^0\n", args);

    return (r & ERROR) == 0 ? 0 : 1;
}
