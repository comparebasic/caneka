extern word HttpV_RangeDef[];
/* specific parsers */
Parser *Parser_Method(struct serve_ctx *sctx, struct serve_req *req);
Parser *Parser_Space(struct serve_ctx *sctx, struct serve_req *req);
Parser *Parser_Path(struct serve_ctx *sctx,  struct serve_req *req);
Parser *Parse_HttpV(struct serve_ctx *sctx,  struct serve_req *req);
