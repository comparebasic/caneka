enum cash_captures {
    CASH_BETWEEN = 1,
    CASH_VALUE,
    CASH_NOOP,
};

enum cash_marks {
    CASH_MARK_START = 1,
};

typedef struct cash {
    Type type;
    MemCtx *m;
    String *s;
    Roebling *rbl;
    Getter get;
    Lookup *presenters;
    Abstract *source;
} Cash;

Cash *Cash_Make(MemCtx *m, Getter get, Abstract *source);
String *Cash_Replace(MemCtx *m, Cash *cash, String *s);
