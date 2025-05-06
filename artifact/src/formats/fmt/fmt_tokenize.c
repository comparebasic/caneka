#include <external.h>
#include <caneka.h>

status FormatFmt_DefSpan(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, Tokenize(m, FORMATTER_INDENT, TOKEN_CLOSE_OUTDENT, TYPE_MESS));
    r |= Lookup_Add(m, Token(m, FORMATTER_LEAD_LINE, TOKEN_CLOSE|TOKEN_CLOSE_OUTDENT, TYPE_MESS));
    r |= Lookup_Add(m, Token(m, FORMATTER_LINE, 0, TYPE_STRVEC));
    return r;
}
