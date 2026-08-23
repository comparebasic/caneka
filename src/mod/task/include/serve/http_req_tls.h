#include <crypto_api.h>
void HttpReq_TlsReadToRbl(MemCh *m, Req *req, Serve *srv);
void HttpReq_TlsFinalize(MemCh *m, Req *req, Serve *srv);
void HttpReq_TlsAccept(MemCh *m, Req *req, Serve *srv);
void HttpReq_TlsWrite(MemCh *m, Req *req, Serve *srv);
void HttpReq_TlsSetup(MemCh *m, Req *req);
void HttpTls_EntSetup(MemCh *m, HostEnt *ent, Node *config);
HandlerDef *HttpTlsReq_DefMake(MemCh *m, 
    Span *handlers, Node *extensions, Abstract *key, Node *config);
