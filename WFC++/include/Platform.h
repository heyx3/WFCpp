#pragma once

#if !defined(WFC_STATIC)
    #define WFC_STATIC 0
#endif

#if defined(_WIN32) || defined(WIN32)

    #define OS_WINDOWS

    //If in an external environment that's already included platform stuff (i.e. Unreal Engine),
    //    skip including Windows stuff again.
    #if !defined(TEXT)
        #define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
        #define NOMINMAX                // Stop conflicts with "min" and "max" macro names
        #include <windows.h>
        //#undef NOMINMAX
        //#undef WIN32_LEAN_AND_MEAN
    #endif

#elif defined(__unix__)

    #define OS_UNIX

#else

    #define OS_UNKNOWN

#endif


//WFC_API: Goes before any class/function/global var that should be visible to users of the DLL.
//WFC_EXIMP: Defined as "extern" if importing a DLL, or nothing otherwise.
#ifdef OS_WINDOWS
    #ifdef WFC_EXPORTS
        #if !defined(WFC_API)
            #define WFC_API __declspec(dllexport)
        #endif
        #define WFC_EXIMP
    #elif WFC_STATIC
        #if !defined(WFC_API)
            #define WFC_API
        #endif
        #define WFC_EXIMP
    #else
        #if !defined(WFC_API)
            #define WFC_API __declspec(dllimport)
        #endif
        #define WFC_EXIMP extern
    #endif
#else
    #define WFC_API
    #define WFC_EXIMP
#endif


//Forcefully include debugging stuff here so that all files will see them.
#if !defined(WFCPP_DEBUG) && !defined(NDEBUG)
    #define WFCPP_DEBUG 1
#endif
#if !defined(WFCPP_ASSERT)
    #define WFCPP_ASSERT(c, ...) assert(c)
#endif
#include "MemoryChecks.inl"