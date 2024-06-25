extern PatCharDef HttpV_RangeDef[];
extern PatCharDef EndNl_RangeDef[];
/* specific parsers */
Parser *Parser_Method(Roebling *sexp);
Parser *Parser_Space(Roebling *sexp);
Parser *Parser_Path(Roebling *sexp);
Parser *Parser_HttpV(Roebling *sexp);
Parser *Parser_EndNl(Roebling *sexp);
Parser *Parser_HColon(Roebling *sexp);
Parser *Parser_HEndAllNl(Roebling *sexp);
Parser *Parser_HEndNl(Roebling *sexp);
