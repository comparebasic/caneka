#include <external.h>
#include <caneka.h>

static status RblSh_RecvCmd(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    if((r & SUCCESS) != 0){
        Roebling_Reset(req->m, req->in.rbl, NULL);
    }

    return h->type.state;
}

static status RblSh_RunCmd(Handler *h, Req *req, Serve *sctx){
    return h->type.state;
}

static status RblSh_Wait(Handler *h, Req *req, Serve *sctx){
    Handler *recv = Handler_Make(req->m, RblSh_RecvCmd, NULL, SOCK_IN);
    Handler_AddPrior(req->m, h, recv);
    return h->type.state;
}

static Handler *RblSh_getHandler(Serve *sctx, Req *req){
    MemCtx *m = req->m;
    Handler *startHandler = Handler_Make(m, RblSh_Wait, NULL, SOCK_IN);
    return startHandler;
}

static Req *RblSh_ReqMake(MemCtx *_m, Serve *sctx){
    Stack(bytes("RblSh_ReqMake"), NULL);
    MemCtx *m = MemCtx_Make();
    IoProto *proto = (IoProto *)IoProto_Make(m, (ProtoDef *)sctx->def);

    Req *req =  Req_Make(m, sctx, (Proto *)proto);
    req->in.rbl = RblShParser_Make(m, 
        (RblShCtx *)sctx->def->source,
        req->in.shelf);

    Return req;
}

static Abstract *RblShCtx_GetVar(Abstract *store, Abstract *key){
    RblShCtx *ctx = (RblShCtx *)as(store, TYPE_RBLSH_CTX);
    return Table_Get(ctx->env, key);
}

static RblShCtx *RblShCtx_Make(MemCtx *m){
    RblShCtx *ctx = MemCtx_Alloc(m, sizeof(RblShCtx));
    ctx->type.of = TYPE_RBLSH_CTX;

    ctx->cash = Cash_Make(m, RblShCtx_GetVar, (Abstract *)ctx);
    ctx->cash->rbl = NULL;

    char buff[PATH_BUFFLEN];
    char *path = getcwd(buff, PATH_BUFFLEN);
    ctx->cwd.s = String_Make(m, bytes(path));
    ctx->cwd.p = String_SplitToSpan(m, ctx->cwd.s, String_Make(m, bytes("/")));

    Debug_Print((void *)ctx->cwd.s, 0, "Cwd(s):", COLOR_PURPLE, FALSE);
    printf("\n");
    Debug_Print((void *)ctx->cwd.p, 0, "Cwd(p):", COLOR_PURPLE, TRUE);
    printf("\n");

    return ctx;
}
static void RblShCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    RblShCtx *ctx = (RblShCtx *)as(a, TYPE_RBLSH_CTX);
    printf("\x1b[%dmRblShCtx<>\x1b[0m", color);
}

static status populateDebugPrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_RBLSH_CTX, (void *)RblShCtx_Print);
    return r;
}

static status debug_Init(MemCtx *m){
    Lookup *funcs = Lookup_Make(m, _TYPE_APPS_START, populateDebugPrint, NULL);
    return Chain_Extend(m, DebugPrintChain, funcs);
}

struct termios orig_tios;
struct termios current_tios;
static status setOrigTios(){
   int r = tcgetattr(STDIN_FILENO, &current_tios);
   if(r != -1){
       memcpy(&orig_tios, &current_tios, sizeof(struct termios));
       return SUCCESS;
   }
   return ERROR;
}

static status rawMode(boolean enable){
    if(enable){
       int r = tcgetattr(STDIN_FILENO, &current_tios);
       if(r != -1){
           current_tios.c_lflag &= ~(ICANON|ISIG);
           /*
           current_tios.c_lflag &= ~(ICANON|ISIG|ECHO);
           current_tios.c_cflag |= (CS8);
           current_tios.c_oflag &= ~(OPOST);
           */
           current_tios.c_cc[VMIN] = 1;
           current_tios.c_cc[VTIME] = 0;
       }
   }else{
       memcpy(&current_tios, &orig_tios, sizeof(struct termios));
   }

   if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &current_tios) != -1){
       return SUCCESS;
   }

   return ERROR; 
}

void Cleanup(Abstract *a){
    rawMode(FALSE);
}

int main(int argc, char **argv){
    Stack(bytes("rblsh:main"), NULL);
    status r = READY;

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);
    debug_Init(m);

    RblShCtx *ctx = RblShCtx_Make(m);

    Handler *h = NULL;
    Req *req = NULL;;
    ProtoDef *def = IoProtoDef_Make(m, (Maker)RblSh_ReqMake);
    def->getHandler = RblSh_getHandler;
    def->source = (Abstract *)ctx;
    Serve *sctx = Serve_Make(m, def);

    Serve_AddFd(sctx, 0);

    setOrigTios();
    rawMode(TRUE);

    Serve_Run(sctx);

    MemCtx_Free(m);
    r |= SUCCESS;
    Return r;
}
