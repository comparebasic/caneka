extern PatCharDef HttpV_RangeDef[];
extern PatCharDef EndNl_RangeDef[];
/* specific parsers */
Parser *Parser_Method(StructExp *sexp);
Parser *Parser_Space(StructExp *sexp);
Parser *Parser_Path(StructExp *sexp);
Parser *Parser_HttpV(StructExp *sexp);
Parser *Parser_EndNl(StructExp *sexp);
Parser *Parser_HColon(StructExp *sexp);
Parser *Parser_HEndAllNl(StructExp *sexp);
Parser *Parser_HEndNl(StructExp *sexp);
