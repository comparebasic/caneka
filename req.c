#include "external.h"
#include "filestore.h"

static char *okBase_cstr = "HTTP/1.1 200 OK\r\nServer: filestore\r\nContent-Length: ";
static String *packageResponse(MemCtx *m, String *content){
    String *s = String_From(m, okBase_cstr);
    String *length_s = String_FromInt(m, (int)content->length); 
    String_Add(m, s, length_s);
    String_AddCstr(m, s, "\r\n\r\n", 4);
    String_Add(m, s, content);

    return s;
}

static char *errBase_cstr = "HTTP/1.1 500 Error\r\nServer: filestore\r\nContent-Length: ";
static String *packageError(MemCtx *m, String *content){
    String *s = String_From(m, errBase_cstr);
    String *length_s = String_FromInt(m, (int)content->length); 
    String_Add(m, s, length_s);
    String_AddCstr(m, s, "\r\n\r\n", 4);
    String_Add(m, s, content);

    return s;
}

char *Method_ToString(int method){
    if(method == METHOD_GET){
        return "GET";
    }else if(method == METHOD_SET){
        return "SET";
    }else if(method == METHOD_UPDATE){
        return "UPDATE";
    }else{
        return "UNKONWN_method";
    }
}

status Req_Parse(Serve *sctx, Req *req, String *s, ParserMaker parsers[]){
    Range find;
    Range_Set(&find, s);
    
    int i = 0;
    ParserMaker pmk = parsers[i];
    int mark = -1;
    boolean escaping = FALSE;
    Parser *prs;
    status r = READY;
    word escape_fl = (CYCLE_ESCAPE|COMPLETE);
    while(pmk != NULL){
        printf("Func %d\n", i);
        prs = pmk(sctx, req);
        if(prs->func == NULL){
            r = COMPLETE | prs->flags;
            printf("skip %d/%d\n", r, prs->flags);
        }else{
            r = prs->func(prs, &find, (void *)req);
            printf("run %d %s\n", r, State_ToString(r));
        }

        if(escaping && ((r & CYCLE_LOOP) != 0)){
            escaping = FALSE;
        }else{
            if((r & escape_fl) == escape_fl){
                escaping = TRUE;
            }else{
                if((r & COMPLETE) != 0){
                    if((r & CYCLE_MARK) != 0){
                        printf(" -> SETTING MARK %d\n", i+1);
                        mark = i;
                    }else if(((r & CYCLE_LOOP) != 0) && mark != -1){
                        printf(" -> BACKT TO LOOP SETTING MARK %d\n", i+1);
                        i = mark; 
                    }else{
                        printf("NO LOOP OR MARK%d\n", i+1);
                    }
                }else{
                    req->state = ERROR;
                    return req->state;
                }
            }
        }
        pmk = parsers[++i];
    }
        
    Debug_Print((void *)req, TYPE_REQ, "Req Parsed ", COLOR_CYAN, TRUE);

    req->state = PROCESSING;
    Serve_NextState(sctx, req);
    return req->state;
}

status Req_Recv(Serve *sctx, Req *req){
    byte buff[SERV_READ_SIZE];
    size_t l = recv(req->fd, buff, SERV_READ_SIZE, 0);
    if(l > 0){
        return Req_Parse(sctx, req, String_Make(req->m, buff), sctx->parsers);
    }

    return NOOP;
}

status Req_Process(Serve *sctx, Req *req){
    req->response = packageResponse(req->m, String_From(req->m, "poo"));
    req->cursor = SCursor_Make(req->m, req->response);

    req->state = RESPONDING;
    Serve_NextState(sctx, req);
    return req->state;
}

status Req_Handle(Serve *sctx, Req *req){
    if(req->state == INCOMING){
        return Req_Recv(sctx, req);
    }

    if(req->state == PROCESSING){
        return Req_Process(sctx, req);
    }

    return NOOP;
}

Req *Req_Make(){
    MemCtx *m = MemCtx_Make();
    Req* req = (Req *)MemCtx_Alloc(m, sizeof(Req));
    req->direction = -1;

    MemCtx_Bind(m, req);

    return req;
}

status Req_SetError(Serve *sctx, Req *req, String *msg){
    req->state = RESPONDING;
    Serve_NextState(sctx, req);
    req->response = packageError(req->m, msg);
    return SUCCESS;
}

