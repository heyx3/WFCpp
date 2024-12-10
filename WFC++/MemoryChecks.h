#pragma once

#include <cstdint>
#include <array>
#include <inttypes.h>
#include <exception>
#include <cstdio>


//To debug heap memory corruption, `#define WFCPP_CHECK_MEMORY 1` before including this library.
//You may also define a printf-like error handler, `WFCPP_CHECK_MEMORY_ERROR(s, ...)`.
//It defaults to `fprintf(stderr, ...)`.
//Then you can call `DEBUGMEM_Validate()` on an instance to check the padding bits for corruption.

//For Intellisense, act as if memory debugging is always on.
#if !defined(WFCPP_CHECK_MEMORY) && (defined(__INTELLISENSE__) || defined(__RESHARPER__))
    #define WFCPP_CHECK_MEMORY 1
#endif

//By default, only enable memory debugging in debug builds.
#if !defined(WFCPP_CHECK_MEMORY)
    #if defined(NDEBUG)
        #define WFCPP_CHECK_MEMORY 0
    #else
        #define WFCPP_CHECK_MEMORY 1
    #endif
#endif

//Make sure there is an error handler.
#if WFCPP_CHECK_MEMORY && !defined(WFCPP_CHECK_MEMORY_ERROR)
    #define WFCPP_CHECK_MEMORY_ERROR(s, ...) \
        fprintf(stderr, "WFCPP: heap corruption detected! " s, ##__VA_ARGS__)
#endif

namespace WFC::DEBUGMEM
{
    //Helper struct representing a set of bytes that should stay at a hard-coded value
    //    unless the heap has been corrupted.
    template<size_t N, const char* (*HeaderMsgGetter)()>
    struct Padding
    {
        std::array<std::uint32_t, N> Data;
        static constexpr uint32_t TestValue = 0xdeadbeef;
        Padding()
        {
            for (size_t i = 0; i < N; ++i)
                Data[i] = TestValue;
        }
        //Returns -1 if none was found.
        int64_t GetFirstFailedIndex() const
        {
            for (size_t i = 0; i < N; ++i)
                if (Data[i] != TestValue)
                    return static_cast<int64_t>(i);
            return -1;
        }
        void Validate() const
        {
            auto firstFailedIdx = GetFirstFailedIndex();
            if (firstFailedIdx >= 0)
            {
                WFCPP_CHECK_MEMORY_ERROR(
                    "Start of %s has corrupted padding value starting at byte %" PRId64 "! "
                        "It should be %#010x but it's %#010x",
                    HeaderMsgGetter(), firstFailedIdx * 4,
                    TestValue, Data[firstFailedIdx]
                );
                throw std::exception{ };
            }
        }
    };

    inline const char* TestPaddingHeaderGetter() { return "TestHere"; }
    using TestPadding = Padding<16, &TestPaddingHeaderGetter>;
    static_assert(
        sizeof(TestPadding) == 16 * 4,
        "16" "-element padding doesn't work in this compiler; it gets padded to be larger"
    );
}

#if WFCPP_CHECK_MEMORY
    //Insert this at the top of a struct to pad it with int32's, to check for bad writes.
    //Must be followed up with the FOOTER version at the bottom of the struct.
    #define WFCPP_MEMORY_CHECK_HEADER(nWordsOfPadding, headerString) \
        static const char* DEBUGMEM_GetHeaderString() { return headerString; } \
        WFC::DEBUGMEM::Padding<(nWordsOfPadding), &DEBUGMEM_GetHeaderString> DEBUGMEM_Header; \
        static_assert(sizeof(decltype(DEBUGMEM_Header) )== 4 * (nWordsOfPadding), \
                      #nWordsOfPadding "-element padding doesn't work; it got padded to some larger size")
    //Insert this at the bottom of a struct that had the HEADER version inserted to the top of it.
    #define WFCPP_MEMORY_CHECK_FOOTER(nWordsOfPadding) \
        WFC::DEBUGMEM::Padding<(nWordsOfPadding), &DEBUGMEM_GetHeaderString> DEBUGMEM_Footer; \
        static_assert(sizeof(decltype(DEBUGMEM_Footer)) == 4 * (nWordsOfPadding), \
                      #nWordsOfPadding "-element padding doesn't work; it got padded to some larger size"); \
        void DEBUGMEM_Validate() const { DEBUGMEM_Header.Validate(); DEBUGMEM_Footer.Validate(); }
#else
    #define WFCPP_MEMORY_CHECK_HEADER(...) 
    #define WFCPP_MEMORY_CHECK_FOOTER(...) \
        template<typename... Args> void DEBUGMEM_Validate(Args... args) const { }
#endif