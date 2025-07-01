typedef struct cash {
    Type type; 
    Span *p;
} Cash;

Cash *Cash_Make(MemCh *m);
Cash *Cash_FromStrVec(MemCh *m, StrVec *v);
