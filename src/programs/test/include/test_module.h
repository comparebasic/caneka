#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include "testsuite.h"
#include "mock_109strings.h"

#ifdef CNKOPT_BASE
    #include "base_tests.h"
#endif
#ifdef CNKOPT_EXT
    #include "ext_tests.h"
#endif
#ifdef CNKOPT_INTER
    #include "inter_tests.h"
#endif
#ifdef CNKOPT_CRYPTO
    #include "cryto_tests.h"
#endif

#endif
