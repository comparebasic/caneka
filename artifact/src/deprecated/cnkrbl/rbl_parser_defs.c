#include <external.h>
#include <caneka.h>

static word rblLabelDef[] = {
    PAT_KO,'(','(',PAT_MANY,'_','_',PAT_MANY,'-','-',PAT_MANY,'A','Z',PAT_MANY,'a','z',PAT_MANY,'0','9',
    PAT_END, '0', '0'
};

static word rblLabelCloseDef[] = {
    PAT_TERM,')',')',
    PAT_END, '0', '0'
};

static word rblBetweenDef[] = {
    PAT_KO|PAT_INVERT, '{', '{', PAT_KO|PAT_TERM, '{', '{',patText,
    PAT_END, '0', '0'
};

static word rblPatStartDef[] = {
    PAT_TERM, '{', '{',
    PAT_END, '0', '0'
};

static word rblPatEndDef[] = {
    PAT_TERM, '}', '}',
    PAT_END, '0', '0'
};

static word rblAnyDef[] = {
    PAT_TERM, '*', '*',
    PAT_END, '0', '0'
};

static word rblManyDef[] = {
    PAT_TERM, '+', '+',
    PAT_END, '0', '0'
};

static word rblNoCaptureDef[] = {
    PAT_TERM, '_', '_',
    PAT_END, '0', '0'
};

static word rblKoDef[] = {
    PAT_TERM, '^', '^',
    PAT_END, '0', '0'
};

static word rblInvertDef[] = {
    PAT_TERM, '!', '!',
    PAT_END, '0', '0'
};

static word rblTextDef[] = {
    PAT_TERM, '.', '.',
    PAT_END, '0', '0'
};
