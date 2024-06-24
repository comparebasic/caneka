enum mark_reserved {
    LOOP_MARK = 0,
    _FIRST_MARK,
};

typedef struct structexp {
    cls type;
    MemCtx *m;
    status state;
    void *source; 
    Span *marks;
    Range range;
    ParserMaker *parsers;
} StructExp;

StructExp *StructExp_Make(MemCtx *m, cls type, ParserMaker *parsers, String *s, void *source);
status StructExp_Run(StructExp *sexp);
