typedef struct int_pair {
    i32 a;
    i32 b;
} IntPair;

typedef struct int_range {
    RangeType type;
} Range;

int Int_FromString(String *s);
i64 I64_FromString(String *s);
Single *Int_Wrapped(MemCtx *m, int i);
Range *Range_Wrapped(MemCtx *m, word r);
Single *I64_Wrapped(MemCtx *m, i64 n);
