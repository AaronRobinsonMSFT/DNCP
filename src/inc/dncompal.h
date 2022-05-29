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

// Typedefs typically provided by Windows' headers
#ifdef DNCOMPAL_TYPEDEFS
    typedef void* LPVOID;
    typedef size_t SIZE_T;

    typedef uint32_t UINT;
    typedef uint32_t ULONG;

#ifdef __cplusplus
    typedef char16_t WCHAR;
#else
    typedef uint16_t WCHAR;
#endif // __cplusplus

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
    #define S_OK            ((HRESULT)0)
    #define S_FALSE         ((HRESULT)1)
    #define E_OUTOFMEMORY   ((HRESULT)0x8007000E)
    #define E_INVALIDARG    ((HRESULT)0x80070057)

    #define E_NOTIMPL       ((HRESULT)0x80004001)
    #define E_NOINTERFACE   ((HRESULT)0x80004002)
    #define CO_E_CLASSSTRING ((HRESULT)0x800401F3)

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

// 00000000-0000-0000-C000-000000000046
extern IID const IID_IUnknown;

// 00000001-0000-0000-C000-000000000046
extern IID const IID_IClassFactory;

#ifdef __cplusplus
    }
#endif // __cplusplus

//
// Interfaces
//

#if defined(__cplusplus) && defined(DNCOMPAL_INTERFACES)

    #define STDMETHODCALLTYPE
    #define __uuidof(x) IID_ ## x

    using REFGUID = GUID const &;
    using REFIID = IID const &;

    struct IUnknown
    {
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid,
            void **ppvObject) = 0;

        virtual ULONG STDMETHODCALLTYPE AddRef() = 0;

        virtual ULONG STDMETHODCALLTYPE Release() = 0;
    };

    struct IClassFactory : public IUnknown
    {
        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
            IUnknown *pUnkOuter,
            REFIID riid,
            void **ppvObject) = 0;

        virtual HRESULT STDMETHODCALLTYPE LockServer(
            BOOL fLock) = 0;
    };

#endif // __cplusplus && DNCOMPAL_INTERFACES
