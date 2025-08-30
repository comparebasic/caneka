typedef struct cash_ctx {
    Type type; 
    Type next;
    Roebling *rbl;
    Abstract *source;
    Iter it;
} TemplCtx;

TemplCtx *TemplCtx_FromCurs(MemCh *m, Cursor *curs, Abstract *source);
TemplCtx *TemplCtx_Make(MemCh *m, Cursor *curs, Abstract *source);
