#if defined(__clang__)
    #define _gen_CC "clang"
    #define _gen_CC_VERSION __clang_major__
#elif defined(__INTEL_COMPILER)
    #define _gen_CC "icc"
    #define _gen_CC_VERSION __INTEL_COMPILER
#elif __GNUC__ 
    #define _gen_CC "gcc"
    #define _gen_CC_VERSION __GNUC__
#elif defined(_MS_VER)
    #define _gen_CC "msvc"
    #define _gen_CC_VERSION _MS_VER
#endif

#define _gen_AR "ar"
