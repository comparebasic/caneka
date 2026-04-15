#include <external.h>
#include <caneka.h>

Tokenize *Tokenize_Make(MemCh *m, word captureKey, word flags, cls typeOf){
    Tokenize *tk = (Tokenize *)MemCh_Alloc(m, sizeof(Tokenize));
    tk->type.of = TYPE_TOKENIZE;
    tk->type.state = flags;
    tk->captureKey = captureKey;
    tk->typeOf = typeOf;
    return tk;
}
