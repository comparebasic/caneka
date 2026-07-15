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

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *uriKey = K(m, "uri");
    Str *destKey = K(m, "dest");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show this help message."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, uriKey, NULL, ARG_OPTIONAL,
        Sv(m, "Url to fetch."));
    Args_Add(cli, destKey, NULL, ARG_OPTIONAL,
        Sv(m, "File to place binseg metadata"));

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, noColorKey)){
        Ansi_SetColor(OutStream, FALSE);
    }

#ifdef CNKOPT_EXT
    StrVec *uriText = Ifc(m, CliArgs_Get(cli, uriKey), TYPE_STRVEC);
    Uri *uri = Uri_Make(m, uriText);
    Str *name = Ifc(m, uri->host, TYPE_STR);
    Str *service = Ifc(m, uri->proto, TYPE_STR);

    args[0] = uri->v;
    args[1] = name;
    args[2] = service;
    args[3] = NULL;
    Out("^y.Requesting -> @ from $:$^0\n", args);

    HostEnt *h = HostEnt_FromName(m, name, service);

    Str *text = S(m, getReq);
    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
    i32 port = Serve_PortByService(uri->proto);
    Conn_InetConnect(bf, h, port);
    Buff_Add(bf, text); 

    HttpReq *req = (HttpReq *)HttpReq_Mk(MemCh_Make(), NULL);
    HttpReq_SetToResponse(req, bf->fd);
    while((req->type.state & (SUCCESS|ERROR)) == 0){
        HttpReq_ReadToRbl(req->m, req, NULL);
    }
    HttpReq_Close(m, req, NULL);

#ifdef CNKOPT_CRYPTO
    Str *sha = Str_DigestAlloc(m);
    Buff *body = Ifc(m, req->body, TYPE_BUFF);
    StrVec_ToSha256(m, body->v, (digest*)sha->bytes);
    sha->length = DIGEST_SIZE;
    Str *hex = Str_ToHex(m, sha);

    args[0] = hex;
    args[1] = NULL;
    Out("^p.ShaHex: @^0\n", args);
#endif
    
    Str *dest = CliArgs_Get(cli, destKey); 
    if(dest != NULL){
        StrVec *path = IoUtil_AbsVec(m, StrVec_From(m, dest));
        StrVec *dir = IoUtil_BasePath(m, path);

        void *ar[] = {
            path,
            dir,
            NULL
        };
        Out("^p.Path:@ dir:@ ^0\n", ar);

        Dir_CheckCreate(m, Ifc(m, dir, TYPE_STR));
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        File_Open(bf, path, O_WRONLY|O_CREAT);
#ifdef CNKOPT_CRYPTO
        Buff_Add(bf, hex);
#else
        struct timespec now;
        Time_Now(&now);
        Buff_Add(bf, Time_ToStr(m, &now));
#endif
        File_Close(bf);
    }

    MemCh_Free(req->m);
#endif

    return (r & ERROR) == 0 ? 0 : 1;
}
