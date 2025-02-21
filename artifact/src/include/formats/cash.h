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
Roebling *Cash_RblMake_rbl(MemCtx *m, Abstract *cash, RblCaptureFunc capture);
String *Cash_Replace(MemCtx *m, Cash *cash, String *s);
status Cash_Capture(word captureKey, int matchIdx, String *s, Abstract *source);
status Cash_Between(MemCtx *m, Roebling *rbl);
status Cash_ValueQuoted(MemCtx *m, Roebling *rbl);
