#include <external.h>
#include <caneka.h>
#include <signer.h>

PatCharDef keyDef = {
    {PAT_KO|PAT_KO_TERM, ':', ':'},
    patText,
    {PAT_END, 0, 0},
};

PatCharDef valueDef = {
    {PAT_KO|PAT_KO_TERM, ';', ';'},
    {PAT_INVERT, ':', ':'}, patText,
    {PAT_END, 0, 0},
};

PatCharDef contentDef = {
    patText,
    {PAT_END, 0, 0},
};

Roebling *SignerIdent_RblMake(MemCtx *m){
    return NULL;
}
