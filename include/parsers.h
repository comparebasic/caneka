typedef status (*ParseFunc)(struct parser *parser, Range *range, void *source);

typedef struct parser {
    cls type;
    word flags;
    void *matches;
    int idx;
    ParseFunc func;
    ParseFunc complete;
} Parser;

typedef Parser *(*ParserMaker)(StructExp *sexp);

Parser *Parser_Make(MemCtx *m, cls type);
Parser *Parser_MakeSingle(MemCtx *m, Match *mt, ParseFunc complete);
Parser *Parser_MakeMulti(MemCtx *m, Match **mt_arr, ParseFunc complete);

/* cycle parsers */
Parser *Parser_Mark(StructExp *sexp);
Parser *Parser_Loop(StructExp *sexp);
Parser *Parser_Break(StructExp *sexp);
