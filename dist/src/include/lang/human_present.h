typedef String *(Presenter)(MemCtx *m, Abstract *a);

String *Time64_Present(MemCtx *m, Abstract *s);
String *I64_Present(MemCtx *m, Abstract *s);
String *Util_Present(MemCtx *m, Abstract *s);
String *Bool_Present(MemCtx *m, Abstract *s);
String *String_Present(MemCtx *m, Abstract *s);
