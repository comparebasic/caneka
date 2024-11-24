enum cash_captures {
    CASH_BETWEEN = 1,
    CASH_VALUE,
};

enum cash_marks {
    CASH_MARK_START = 1,
};

typedef struct cash {
    Type type;
    MemCtx *m;
    Span *tbl;
    String *s;
    Roebling *rbl;
} Cash;

Cash *Cash_Make(MemCtx *m);
String *Cash_Replace(MemCtx *m, Cash *cash, String *s);
