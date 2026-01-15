typedef struct itinerary {
    Type type;
    Type objType;
    Lookup *positions;
} Itin;

Itin *Itin_Make(MemCh *m, cls typeOf);
Iter *ItinIt_Make(MemCh *m, void *a);
