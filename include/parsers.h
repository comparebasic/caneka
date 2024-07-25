enum parser_cmds {
    BREAK = -2,
};

typedef status (*ParseFunc)(struct parser *parser, Range *range, void *source);

typedef struct parser {
    Type type;
    word flags;
    union {
        Match *single;
        Match **array;
    } match;
    Match *ko;
    int idx;
    ParseFunc func;
    int jump;
    int failJump;
    ParseFunc complete;
} Parser; /*rp*/

Parser *Parser_Make(MemCtx *m, cls type);
Parser *Parser_MakeSingle(MemCtx *m, Match *mt, ParseFunc complete);
Parser *Parser_MakeMulti(MemCtx *m, Match **mt_arr, ParseFunc complete);
Abstract *Parser_GetMatchKey(Parser *prs);
Abstract *Parser_GetContent(Parser *prs, Range *range, void *source);

/* cycle parsers */
Parser *Parser_Mark(Roebling *sexp);
Parser *Parser_Loop(Roebling *sexp);
Parser *Parser_Break(Roebling *sexp);

/* parser constructors */
Parser *Parser_StringSet(MemCtx *m, word flags, ParseFunc complete, ...);
Parser *Parser_StringLookup(MemCtx *m, word flags, ParseFunc complete, Lookup *lb);
Parser *Parser_String(MemCtx *m, word flags, ParseFunc complete, byte *b);
/* utils */
Match *Parser_GetMatch(Parser *prs);
