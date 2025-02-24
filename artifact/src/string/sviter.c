#include <external.h>
#include <caneka.h>

status SvIter_Init(SvIter *it, StrVec *vh){
    memset(it, 0, sizeof(SvIter));
    it->type.of = TYPE_STRVEC_ITER;
    it->idx = -1;
    it->vh = vh;
    it->ve = (StrVecEntry*)vh+1;
    return SUCCESS;
}

status SvIter_Next(SvIter *it){
    if(it->idx == -1){
        printf("to first\n");
        it->idx++;
    }else if(it->ve == it->vh->last){
        it->type.state |= END;
    }else{
        printf("to next\n");
        if(it->ve->jump != NULL){
            it->ve = it->ve->jump;
        }else{
            it->ve++;
        }
        it->idx++;
    }
    if(it->ve == it->vh->last){
        it->type.state |= FLAG_ITER_LAST;
    }
    return it->type.state;
}

StrVecEntry *SvIter_Get(SvIter *it){
    return it->ve;    
}
