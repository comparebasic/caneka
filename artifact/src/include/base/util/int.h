typedef struct int_pair {
    i32 a;
    i32 b;
} IntPair;

typedef struct int_range {
    RangeType type;
} Range;

i32 Int_FromStr(Str *s);
i64 I64_FromStr(Str *s);
