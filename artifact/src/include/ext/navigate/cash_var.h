typedef struct cash_var {
    Type type; 
    Abstract *token;
} CashVar;

CashVar *CashVar_Make(MemCh *m);
