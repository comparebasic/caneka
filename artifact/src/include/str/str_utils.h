extern struct Lookup *ToStreamLookup;
status StrUtils_Init(MemCh *m);
Str *Str_FromTyped(MemCh *m, void *v, cls type);
Str *Str_FromAbs(MemCh *m, Abstract *a);
StrVec *StrVec_FromAbs(MemCh *m, Abstract *a);
