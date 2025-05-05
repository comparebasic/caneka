#include <external.h>
#include <caneka.h>

status FormatFmt_DefSpan(MemCtx *m, Lookup *lk){
    status r = READY;
    cls indentTypes[] = {TYPE_MESS, 0};
    r |= Lookup_Add(m, Token(m, FORMATTER_INDENT, TOKEN_CLOSE_OUTDENT, indentTypes));
    cls leadLineTypes[] = {TYPE_MESS, 0};
    r |= Lookup_Add(m, Token(m, FORMATTER_LEAD_LINE, TOKEN_CLOSE|TOKEN_CLOSE_OUTDENT, indentTypes));
    cls lineTypes[] = {TYPE_STRVEC, 0};
    r |= Lookup_Add(m, Token(m, FORMATTER_LINE, 0, indentTypes));
    return r;
}
