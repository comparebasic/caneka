Parser *Parser_StringSet(MemCtx *m, word flags, ParseFunc complete, ...);
Parser *Parser_StringLookup(MemCtx *m, word flags, ParseFunc complete, Lookup *lb);
Parser *Parser_String(MemCtx *m, word flags, ParseFunc complete, byte *b);
