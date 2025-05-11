enum relation_flags {
    RELATION_ROW_START = 1 << 8,
    RELATION_ROW_END = 1 << 9,
};

typedef struct relation {
    Type type;
    i16 _;
    i16 stride;
    Abstract **headers;
    Iter it;
} Relation;

Relation *Relation_Make(MemCh *m, i16 stride, Abstract **headers);
status Relation_HeadFromValues(Relation *rel);
status Relation_Start(Relation *rel);
status Relation_Next(Relation *rel);
status Relation_SetValue(Relation *rel, i16 col, i16 row, Abstract *value);
