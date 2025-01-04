#include <external.h>
#include <caneka.h>

static FmtCnf _configs[] = {
    {
        CNK_LANG_RBL_START,
        0,
        FMT_DEF_TO_ON_CLOSE,
        "",
        NULL,
        CnkRbl_Out,  
        NULL,
        -1
    },
    {
        CNK_LANG_RBL_OPEN_CLOSE,
        0,
        FMT_DEF_TO_ON_CLOSE,
        "",
        NULL,
        NULL,  
        NULL,
        -1
    },
    {
        CNK_LANG_RBL_PAT_KEY,
        0,
        FMT_DEF_INDENT,
        "",
        NULL,
        CnkRbl_PatKeyOpen,  
        NULL,
        -1
    },
    {
        CNK_LANG_RBL_PAT_KEY_CLOSE,
        0,
        FMT_DEF_OUTDENT,
        "",
        NULL,
        CnkRbl_PatClose,  
        NULL,
        -1
    },
    {
        CNK_LANG_RBL_PAT,
        0,
        0,
        "pat",
        NULL,
        CnkRbl_Pat,  
        NULL,
        -1
    },
    {
        0,
        0,
        0,
        NULL,
        NULL,
        NULL,
        NULL,
        -1
    },
};

FmtCnf *cnk_roebling_configs = _configs;
