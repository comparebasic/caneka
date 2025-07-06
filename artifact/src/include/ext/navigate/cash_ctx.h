typedef struct cash_ctx {
    Type type; 
    Roebling *rbl;
    Abstract *source;
    Iter it;
} CashCtx;

CashCtx *CashCtx_FromCurs(MemCh *m, Cursor *curs, Abstract *source);
CashCtx *CashCtx_Make(MemCh *m, Cursor *curs, Abstract *source);
