typedef struct slate {
    Type type;
    RangeType rangeType;
    i16 *available;
    i16 *next;
    void *slots;
} Slate;

i16 Slate_Add(MemCh *m, Slate *sl, void *item);
void Slate_Remove(MemCh *m, Slate *sl, i16 idx);
Slate *Slate_Make(MemCh *m, i16 slots);
