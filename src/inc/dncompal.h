// Copyright 2022 Aaron R Robinson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Perform platform check
#ifdef _MSC_VER
    #define DNCOMPAL_WINDOWS
#endif

// Typedefs typically provided by Windows' headers
#ifdef DNCOMPAL_TYPEDEFS
    typedef void* LPVOID;
    typedef void const* LPCVOID;
    typedef uintptr_t UINT_PTR;
    typedef size_t SIZE_T;

    typedef uint8_t BYTE;
    typedef uint16_t USHORT;
    typedef int32_t UINT;
    typedef int32_t LONG;
    typedef uint32_t ULONG;
    typedef uint32_t ULONG32;
    typedef uint32_t DWORD;
    typedef uint64_t UINT64;
    typedef uint64_t ULONG64;
    typedef uint64_t ULONGLONG;

#ifdef __cplusplus
    typedef char16_t WCHAR;
#else
    typedef uint16_t WCHAR;
#endif // __cplusplus

    typedef WCHAR* LPWSTR;
    typedef WCHAR const* LPCWSTR;

    typedef WCHAR OLECHAR;
    typedef OLECHAR* LPOLESTR;
    typedef OLECHAR const* LPCOLESTR;
    typedef OLECHAR* BSTR;

    typedef int32_t BOOL;
    #define TRUE ((BOOL)1)
    #define FALSE ((BOOL)0)

    typedef int16_t VARIANT_BOOL;
    #define VARIANT_TRUE ((VARIANT_BOOL)-1)
    #define VARIANT_FALSE ((VARIANT_BOOL)0)

    typedef uint32_t HRESULT;

    typedef struct
    {
        uint32_t  Data1;
        uint16_t  Data2;
        uint16_t  Data3;
        uint8_t   Data4[8];
    } GUID;

    typedef GUID IID;
#endif // DNCOMPAL_TYPEDEFS

#ifdef __cplusplus
    extern "C"
    {
#endif // __cplusplus

//
// Memory allocators
//
LPVOID PAL_CoTaskMemAlloc(SIZE_T);
void PAL_CoTaskMemFree(LPVOID);

//
// Strings
//

// This macro is used to standardize wide character string literals used in .NET.
#ifdef DNCOMPAL_WINDOWS
    #define W(str)  L ## str
#else
    #define W(str)  u ## str
#endif

// BSTR
BSTR PAL_SysAllocString(LPCOLESTR);
BSTR PAL_SysAllocStringLen(LPCOLESTR, UINT);
BSTR PAL_SysAllocStringByteLen(char const*, UINT);
void PAL_SysFreeString(BSTR);
UINT PAL_SysStringLen(BSTR);
UINT PAL_SysStringByteLen(BSTR);

//
// GUIDs
//

BOOL PAL_IsEqualGUID(GUID const*, GUID const*);

int32_t PAL_StringFromGUID2(GUID const*, LPOLESTR, int32_t);
HRESULT PAL_IIDFromString(LPCOLESTR, IID*);

#ifdef __cplusplus
    }
#endif // __cplusplus

//
// Windows headers
//

#ifdef DNCOMPAL_WINHDRS
    #include <winerror.h>

    #if defined(__cplusplus)

        #define EXTERN_C extern "C"

        using REFGUID = GUID const&;
        using IID = GUID;
        using REFIID = IID const&;
        using CLSID = GUID;
        using REFCLSID = CLSID const&;

        #define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8) \
            EXTERN_C __declspec(selectany) IID const itf = {l1,s1,s2,{c1,c2,c3,c4,c5,c6,c7,c8}}

        inline bool operator==(REFGUID a, REFGUID b)
        {
            return FALSE != PAL_IsEqualGUID(&a, &b);
        }

        // sal
        #define _In_
        #define _Out_
        #define _Out_opt_
        #define _Out_writes_to_opt_(x,y)
        #define _Out_writes_to_(x,y)
        #define _COM_Outptr_
        #define __RPC__deref_out_opt

        // COM Interface definitions
        #define __uuidof(type) IID_##type
        #define interface struct
        #define DECLSPEC_UUID(x)
        #define DECLSPEC_NOVTABLE
        #define MIDL_INTERFACE(x)                       struct DECLSPEC_UUID(x) DECLSPEC_NOVTABLE
        #define DECLARE_INTERFACE_(iface, baseiface)    interface DECLSPEC_NOVTABLE iface : public baseiface
        #define STDMETHODCALLTYPE
        #define STDMETHOD(method)       virtual HRESULT STDMETHODCALLTYPE method
        #define STDMETHOD_(type,method) virtual type STDMETHODCALLTYPE method

        #define UNALIGNED
        #define PURE = 0

        #include <unknwn.h>

        // Unusable COM and RPC types
        interface ITypeInfo;
        interface IStream;
        struct VARIANT;
        interface IRpcChannelBuffer;
        using RPC_IF_HANDLE = void*;

        // Unusable Win32 types
        using LPDEBUG_EVENT = SIZE_T;
        using LPSTARTUPINFOW = SIZE_T;
        using LPPROCESS_INFORMATION = SIZE_T;
        using LPSECURITY_ATTRIBUTES = SIZE_T;

    #endif // __cplusplus
#endif // DNCOMPAL_INTERFACES
