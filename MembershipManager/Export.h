//#ifndef exportFile_h_Included
//#define exportFile_h_Included 

#ifdef DLL_STATE
#    undef DLL_STATE
#endif
#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#    ifdef MAKE_DLL_MembershipManager
#        define DLL_STATE __declspec(dllexport)
#    else
#        define DLL_STATE __declspec(dllimport)
#    endif
#else
#    define DLL_STATE
#endif

//#endif


