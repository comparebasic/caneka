typedef struct single  {
    Type type;
    union {
        byte b;
        word w;
        quad i;
        util value;
        Abstract *a;
        void *ptr;
        DoFunc dof;
    } val;
} Single;

Single *Single_Clone(MemCh *m, Abstract *og);
Single *Single_Ptr(MemCh *m, void *ptr);
Single *Bool_Wrapped(MemCh *m, int n);
Single *Do_Wrapped(MemCh *m, DoFunc dof);
Single *Maker_Wrapped(MemCh *m, Maker mk);
Single *Int_Wrapped(MemCh *m, int n);
Single *I64_Wrapped(MemCh *m, i64 n);
Range *Range_Wrapped(MemCh *m, word r);
Single *Single_Cstr(MemCh *m, char *cstr);
