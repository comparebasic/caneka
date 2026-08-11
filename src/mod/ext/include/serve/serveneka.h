status HttpReq_Stats(MemCh *m, Req *req, Serve *srv);
status Serveneka_Prepare(MemCh *m, Req *req, Serve *srv);
Abstract *EndPointKey_From(MemCh *m, Table *props);
void Serveneka_Init(MemCh *m);
void Serveneka_Serve(MemCh *m, Node *config);
