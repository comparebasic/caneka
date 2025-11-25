#include <external.h>
#include <caneka.h>

status Histo_FeedVec(Histo *hst, StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        Histo_Feed(hst, s);
    }
    return hst->type.state;
}

status Histo_Feed(Histo *hst, Str *s){
    byte *b = s->bytes;
    byte *end = s->bytes+s->length-1;
    while(b <= end){
        byte c = *b;
        hst->total++;
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
            hst->alpha++;
        }else if((c >= '0' && c <= '9')){
            hst->num++;
        }else if(c == ' ' || c == '\t' || c == '\r' || c == '\n'){
            hst->whitespace++;
        }else if((c < 32) || c == 127){
            hst->control++;
            hst->type.state |= HISTO_CONTROL;
            if(hst->type.state & HISTO_CONTENT_ENFORCE){
                hst->type.state |= ERROR;
                break;
            }
        }else if(c < 127){
            hst->punctuation++;
            hst->ratio = (float)hst->punctuation / (float)(hst->alpha+hst->upper);
            if(HISTO_MIN && (hst->ratio > HISTO_CODE_LEVEL)){
                hst->type.state |= HISTO_CODE;
                if(hst->type.state & HISTO_CONTENT_ENFORCE){
                    hst->type.state |= ERROR;
                    break;
                }
            }else{
                hst->type.state &= ~HISTO_CODE;
            }
        }else{
            hst->upper++;
            hst->type.state |= HISTO_UNICODE;
        }
        b++;
    }
    return ZERO;
}

Histo *Histo_Make(MemCh *m){
    Histo *hst = MemCh_AllocOf(m, sizeof(Histo), TYPE_HISTO);
    hst->type.of = TYPE_HISTO;
    return hst;
}
