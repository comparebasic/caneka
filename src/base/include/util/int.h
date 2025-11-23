typedef struct int_pair {
    i32 a;
    i32 b;
} IntPair;

typedef struct int_range {
    RangeType type;
} Range;

i32 Int_FromStr(Str *s);
i64 I64_FromStr(Str *s);
i64 Str_I64OnBytes(byte **_b, i64 i);
i64 Str_AddI64(Str *s, i64 i);
Str *Str_FromI64(MemCh *m, i64 i);
Str *Str_FromI64Pad(MemCh *m, i64 i, i32 pad);
i64 Str_AddIByte(Str *s, byte i);
