#include <external.h>
#include <caneka.h>

NGram *NGram_Make(MemCh *m,  Buff *bf, i16 stride, void *source){
    NGram *ng = MemCh_AllocOf(m, sizeof(NGram), TYPE_NGRAM);
    ng->type.of = TYPE_NGRAM;
    ng->objRange.range = stride;
    ng->bf = bf;
    ng->source = source;
    ng->s = Str_Make(m, sizeof(util) + (sizeof(NVal) * stride));
    return ng;
}
