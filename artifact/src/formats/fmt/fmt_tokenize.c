#include <external.h>
#include <caneka.h>

status FormatFmt_DefSpan(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, FORMATTER_NEXT,
        Tokenize_Make(m, FORMATTER_NEXT, TOKEN_CLOSE_OUTDENT, 0));
    r |= Lookup_Add(m, lk, FORMATTER_INDENT,
        Tokenize_Make(m, FORMATTER_INDENT,
            TOKEN_ATTR_VALUE|TOKEN_CLOSE_OUTDENT, TYPE_NODE));
    r |= Lookup_Add(m, lk, FORMATTER_LINE,
        Tokenize_Make(m, FORMATTER_LINE, 0, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_BULLET,
        Tokenize_Make(m, FORMATTER_BULLET,
            TOKEN_ATTR_VALUE|TOKEN_NO_COMBINE, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_END,
        Tokenize_Make(m, FORMATTER_END, TOKEN_CLOSE_OUTDENT, 0));
    return r;
}
