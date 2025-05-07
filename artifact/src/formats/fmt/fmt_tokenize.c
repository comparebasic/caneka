#include <external.h>
#include <caneka.h>

status FormatFmt_DefSpan(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, FORMATTER_INDENT,
        Tokenize_Make(m, FORMATTER_INDENT, TOKEN_CLOSE_OUTDENT, TYPE_NODE));
    r |= Lookup_Add(m, lk, FORMATTER_LEAD_LINE,
        Tokenize_Make(m, FORMATTER_LEAD_LINE, TOKEN_CLOSE|TOKEN_CLOSE_OUTDENT, TYPE_MESS));
    r |= Lookup_Add(m, lk, FORMATTER_LINE,
        Tokenize_Make(m, FORMATTER_LINE, 0, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_END,
        Tokenize_Make(m, FORMATTER_END, TOKEN_CLOSE_OUTDENT, 0));
    return r;
}
