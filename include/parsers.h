typedef status (*ParseFunc)(struct parser *parser, Range *range, void *source);

typedef struct parser {
    Type type;
    word flags;
    union {
        Match *single;
        Match **array;
    } match;
    int idx;
    ParseFunc func;
    ParseFunc complete;
} Parser; /*rp*/

Parser *Parser_Make(MemCtx *m, cls type);
Parser *Parser_MakeSingle(MemCtx *m, Match *mt, ParseFunc complete);
Parser *Parser_MakeMulti(MemCtx *m, Match **mt_arr, ParseFunc complete);

/* cycle parsers */
Parser *Parser_Mark(Roebling *sexp);
Parser *Parser_Loop(Roebling *sexp);
Parser *Parser_Break(Roebling *sexp);
