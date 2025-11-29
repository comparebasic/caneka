#ifndef __CNK_HEADER
#define __CNK_HEADER

#include "base/module.h"
#ifdef CNK_EXT
    #include <ext/module.h>
#endif
#ifdef CNK_INTER
    #include <inter/module.h>
#endif
#ifdef CNK_CRYPTO
    #include <crypto.h>
#endif
#ifdef CNK_NACL
    #include <cnk_nacl_crypto.h>
#endif

#endif
