#ifndef __CNK_HEADER
#define __CNK_HEADER

#include "config.h"
#include <base_module.h>
#ifdef CNK_EXT
    #include <ext_module.h>
#endif
#ifdef CNK_LANG
    #include <lang_module.h>
#endif
#ifdef CNK_WWW
    #include <www_module.h>
#endif
#ifdef CNK_CRYPTO
    #include <crypto.h>
#endif
#ifdef CNK_NACL
    #include <cnk_nacl_crypto.h>
#endif

#endif
