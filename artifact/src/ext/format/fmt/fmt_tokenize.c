#include <external.h>
#include <caneka.h>

status FormatFmt_DefSpan(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, FORMATTER_NEXT,
        Tokenize_Make(m, FORMATTER_NEXT, TOKEN_OUTDENT|TOKEN_NO_CONTENT, ZERO));
    r |= Lookup_Add(m, lk, FORMATTER_INDENT,
        Tokenize_Make(m, FORMATTER_INDENT,
            TOKEN_ATTR_VALUE|TOKEN_OUTDENT, TYPE_NODE));
    r |= Lookup_Add(m, lk, FORMATTER_LINE,
        Tokenize_Make(m, FORMATTER_LINE, TOKEN_SEPERATE, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_BULLET,
        Tokenize_Make(m, FORMATTER_BULLET,
            TOKEN_NO_CONTENT|TOKEN_BY_TYPE|TOKEN_NO_COMBINE, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_END,
        Tokenize_Make(m, FORMATTER_END, TOKEN_OUTDENT, ZERO));
    r |= Lookup_Add(m, lk, FORMATTER_TABLE,
        Tokenize_Make(m, FORMATTER_TABLE, 
            TOKEN_NO_COMBINE, TYPE_RELATION));
    r |= Lookup_Add(m, lk, FORMATTER_TABLE_VALUE,
        Tokenize_Make(m, FORMATTER_TABLE_VALUE, TOKEN_NO_COMBINE, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_LAST_TABLE_VALUE,
        Tokenize_Make(m, FORMATTER_LAST_TABLE_VALUE, LAST|TOKEN_NO_COMBINE, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_PARAGRAPH, 
        Tokenize_Make(m, FORMATTER_PARAGRAPH, TOKEN_BY_TYPE, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_KEY, 
        Tokenize_Make(m, FORMATTER_KEY, TOKEN_BY_TYPE, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_TAG, 
        Tokenize_Make(m, FORMATTER_TAG, TOKEN_ATTR_VALUE|TOKEN_INLINE|TOKEN_BY_TYPE, TYPE_NODE));
    r |= Lookup_Add(m, lk, FORMATTER_LABEL, 
        Tokenize_Make(m, FORMATTER_LABEL, TOKEN_ATTR_VALUE, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_URL, 
        Tokenize_Make(m, FORMATTER_URL, TOKEN_ATTR_VALUE, TYPE_STRVEC));
    r |= Lookup_Add(m, lk, FORMATTER_CLASS, 
        Tokenize_Make(m, FORMATTER_CLASS, TOKEN_ATTR_VALUE, ZERO));

    return r;
}
