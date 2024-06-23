typedef status (*ParseFunc)(struct parser *parser, Range *range, void *source);

typedef struct parser {
    cls type;
    void *matches;
    int idx;
    ParseFunc func;
    ParseFunc complete;
} Parser;

typedef Parser *(*ParserMaker)(struct serve_ctx *sctx, struct serve_req *req);

Parser *Parser_Make(MemCtx *m, cls type);
Parser *Parser_MakeSingle(MemCtx *m, Match *mt, ParseFunc complete);
Parser *Parser_MakeMulti(MemCtx *m, Match **mt_arr, ParseFunc complete);
