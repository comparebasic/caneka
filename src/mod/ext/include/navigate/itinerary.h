typedef struct itinerary {
    Type type;
    Type objType;
    Lookup *positions;
} Itin;

Itin *Itin_Make(MemCh *m, cls typeOf);
void *Itin_GetByType(Iter *it, cls typeOf);
status Itin_IterAdd(Iter *it, void *value);
Iter *ItinIt_Make(MemCh *m, void *a);
void ItinIt_Init(Iter *it, Span *p);
