typedef struct relation {
    Type type;
    i16 dims;
    i16 stride;
    Abstract **headers;
    Span *p;
} Relation;

Relation *Relation_Make(MemCh *m, i16 dims, i16 stride, Abstract **headers);
status Relation_HeadFromValues(Relation *rel);
status Relation_SetValue(Relation *rel, i16 col, i16 row, Abstract *value);
