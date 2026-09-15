typedef struct arr {
    Type type;
    RangeType rangeType;
    void **data;
} TypedArr;

void **Span_ToArr(MemCh *m, Span *p);
void **Arr_Make(MemCh *m, i32 nvalues);

TypedArr *TypedArr_Make(MemCh *m, cls typeOf, i16 size);
void TypedArr_Set(MemCh *m, TypedArr *arr, i32 idx, void *value);
void *TypedArr_Get(MemCh *m, TypedArr *arr, i32 idx);
