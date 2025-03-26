typedef struct int_pair {
    i32 a;
    i32 b;
} IntPair;

typedef struct int_range {
    RangeType type;
} Range;

int Int_FromStr(Str *s);
i64 I64_FromStr(Str *s);
Single *Int_Wrapped(MemCh *m, int i);
Range *Range_Wrapped(MemCh *m, word r);
Single *I64_Wrapped(MemCh *m, i64 n);
