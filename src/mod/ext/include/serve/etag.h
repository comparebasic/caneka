#define ETAG_MAX (MAX_BASE10+sizeof(quad)+sizeof(util)+3)
Str *Etag(MemCh *m, i32 idx, Str *name, util parity);
