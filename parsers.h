typedef status (*ParseFunc)(struct parser *parser, struct serve_req *req, Range *range);

typedef struct parser {
    cls type;
    void *matches;
    ParseFunc func;
} Parser;

typedef Parser *(*ParserMaker)(struct serve_ctx *sctx, struct serve_req *req);

Parser *Parser_Make(MemCtx *m, cls type);
Parser *Parser_MakeSingle(MemCtx *m, Match *mt);
Parser *Parser_MakeMulti(MemCtx *m, Match **mt_arr);

/* specific parsers */
Parser *Parser_Method(struct serve_ctx *sctx, struct serve_req *req);
Parser *Parser_Space(struct serve_ctx *sctx, struct serve_req *req);
Parser *Parser_Path(struct serve_ctx *sctx,  struct serve_req *req);
