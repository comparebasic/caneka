#define ETAG_MAX (MAX_BASE10+sizeof(quad)+sizeof(util)+3)
#define ETAG_MIN (1+sizeof(quad)+sizeof(util)+3)
Inst *Etag(MemCh *m, i32 idx, StrVec *name, util parity);
Str *Etag_ToStr(Inst *etag);
boolean Etag_CompareStr(Inst *etag, Str *etagStr);
status Etag_ClsInit(MemCh *m);
