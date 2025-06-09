extern Lookup *EqualsLookup;

typedef boolean (*EqFunc)(Abstract *a, Abstract *b);
status Equals_Init(MemCh *m);
boolean Equals(Abstract *a, Abstract *b);
