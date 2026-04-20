typedef struct cash_jump {
    Type type;
    Type objType;
    i32 idx;
} Jump;

void Cash_Out(Span *cash, Buff *bf, void *data);
Span *Cash_Prepare(MemCh *m, Cursor *curs);
Roebling *CashParser_Make(MemCh *m, Cursor *curs, cls instTypeOf);
status Cash_ToSInit(MemCh *m, Lookup *lk);
Jump *Jump_Make(MemCh *m, cls typeOf);
void Jump_FindSource(Jump *jmp, Iter *it);
