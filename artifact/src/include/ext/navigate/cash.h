typedef struct cash {
    Type type; 
    Span *p;
} Cash;

typedef struct cash_ctx {
    Type type; 
    Roebling *rbl;
    Cash *cash;
    Abstract *source;
} CashCtx;

Cash *Cash_Make(MemCh *m);
Cash *Cash_FromStrVec(MemCh *m, StrVec *v);
