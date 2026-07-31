#include <crypto_api.h>
void HttpReq_TlsReadToRbl(MemCh *m, Req *req, Serve *srv);
void HttpReq_TlsFinalize(MemCh *m, Req *req, Serve *srv);
void HttpReq_TlsAccept(MemCh *m, Req *req, Serve *srv);
void HttpReq_TlsWrite(MemCh *m, Req *req, Serve *srv);
HandlerDef *HttpTlsReq_DefMake(MemCh *m);
