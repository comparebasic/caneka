typedef struct single  {
    Type type;
    Type objType;
    union {
        byte b;
        i16 w;
        i32 i;
        util value;
        Abstract *a;
        void *ptr;
        DoFunc dof;
    } val;
} Single;

Single *Single_Clone(MemCh *m, Abstract *og);
Single *Ptr_Wrapped(MemCh *m, void *ptr, cls typeOf);
Single *Bool_Wrapped(MemCh *m, int n);
Single *Do_Wrapped(MemCh *m, DoFunc dof);
Single *Maker_Wrapped(MemCh *m, Maker mk);
Single *I32_Wrapped(MemCh *m, i32 n);
Single *MemCount_Wrapped(MemCh *m, util u);
Single *I64_Wrapped(MemCh *m, i64 n);
Single *Cstr_Wrapped(MemCh *m, char *cstr);
Single *I16_Wrapped(MemCh *m, word w);
Single *I8_Wrapped(MemCh *m, i8 b);
Range *Range_Wrapped(MemCh *m, word r);
