#ifdef DLL_STATE
#    undef DLL_STATE
#endif
#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#    ifdef MAKE_DLL_IntactInfra
#        define DLL_STATE __declspec(dllexport)
#    else
#        define DLL_STATE __declspec(dllimport)
#    endif
#else
#    define DLL_STATE
#endif
