extern PatCharDef HttpV_RangeDef[];
extern PatCharDef EndNl_RangeDef[];
/* specific parsers */
Parser *Parser_Method(struct serve_ctx *sctx, struct serve_req *req);
Parser *Parser_Space(struct serve_ctx *sctx, struct serve_req *req);
Parser *Parser_Path(struct serve_ctx *sctx,  struct serve_req *req);
Parser *Parser_HttpV(struct serve_ctx *sctx,  struct serve_req *req);
Parser *Parser_EndNl(struct serve_ctx *sctx,  struct serve_req *req);
Parser *Parser_HColon(struct serve_ctx *sctx,  struct serve_req *req);
Parser *Parser_HEndAllNl(struct serve_ctx *sctx,  struct serve_req *req);
Parser *Parser_HEndNl(struct serve_ctx *sctx,  struct serve_req *req);
