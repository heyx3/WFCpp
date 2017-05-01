#pragma once


#if defined(_WIN32) || defined(WIN32)

#define OS_WINDOWS

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#define NOMINMAX                // Stop conflicts with "min" and "max" macro names
// Windows Header Files:
#include <windows.h>
//#undef NOMINMAX
//#undef WIN32_LEAN_AND_MEAN

#elif defined(__unix__)

#define OS_UNIX

#endif


//WFC_API: Goes before any class/function/global var that should be visible to users of the DLL.
//WFC_EXIMP: Defined as "extern" if importing a DLL, or nothing otherwise.
#ifdef OS_WINDOWS
    #ifdef WFC_EXPORTS
        #define WFC_API __declspec(dllexport)
        #define WFC_EXIMP
    #elif WFC_STATIC
        #define WFC_API
        #define WFC_EXIMP
    #else
        #define WFC_API __declspec(dllimport)
        #define WFC_EXIMP extern
    #endif
#else
    #define WFC_API
    #define WFC_EXIMP
#endif