typedef struct int_pair {
    i32 a;
    i32 b;
} IntPair;

int Int_FromString(String *s);
i64 I64_FromString(String *s);
Single *Int_Wrapped(MemCtx *m, int i);
Single *I64_Wrapped(MemCtx *m, i64 n);
