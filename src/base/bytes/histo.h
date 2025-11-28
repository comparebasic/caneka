#define HISTO_CODE_LEVEL 0.18f
#define HISTO_MIN 4 

enum histo_flags {
    HISTO_CONTENT_ENFORCE = 1 << 8,
    HISTO_CODE = 1 << 9,
    HISTO_CONTROL = 1 << 10,
    HISTO_UNICODE = 1 << 11,
};

typedef struct histo {
    Type type;
    i64 total;
    float ratio;
    i64 alpha;
    i64 num;
    i64 whitespace;
    i64 punctuation;
    i64 control;
    i64 upper;
} Histo;

Histo *Histo_Make(MemCh *m);
status Histo_Feed(Histo *hst, Str *s);
status Histo_ToRatio(Histo *hst);
