enum single_flags {
    SINGLE_STRVEC_TO_TYPE = 1 << 8,
};

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

Single *True();
Single *False();
Single *Single_Clone(MemCh *m, Abstract *og);
boolean Single_Equals(Single *a, Single *b);
Single *Ptr_Wrapped(MemCh *m, void *ptr, cls typeOf);
Single *Bool_Wrapped(MemCh *m, int n);
Single *Do_Wrapped(MemCh *m, DoFunc dof);
Single *Maker_Wrapped(MemCh *m, Maker mk);
Single *I32_Wrapped(MemCh *m, i32 n);
Single *MemCount_Wrapped(MemCh *m, util u);
Single *I64_Wrapped(MemCh *m, i64 n);
Single *Util_Wrapped(MemCh *m, util u);
Single *Cstr_Wrapped(MemCh *m, char *cstr);
Single *I16_Wrapped(MemCh *m, word w);
Single *I8_Wrapped(MemCh *m, i8 b);
Single *B_Wrapped(MemCh *m, byte b, cls typeOf, word state);
Range *Range_Wrapped(MemCh *m, word r);
#define Abs_FromPtrOfType(sg, t) ((sg) != NULL && \
    (sg)->type.of == TYPE_WRAPPED_PTR \
    && ((Single *)sg)->objType.of == (t))
