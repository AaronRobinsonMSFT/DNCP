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

    typedef uint16_t WCHAR;
    typedef WCHAR OLECHAR;
    typedef OLECHAR* LPOLESTR;
    typedef OLECHAR const* LPCOLESTR;
    typedef WCHAR* BSTR;

    typedef int16_t VARIANT_BOOL;
    #define VARIANT_TRUE ((VARIANT_BOOL)-1)
    #define VARIANT_FALSE ((VARIANT_BOOL)0)

    typedef uint32_t HRESULT;
    #define S_OK            ((HRESULT)0)
    #define E_OUTOFMEMORY   ((HRESULT)0x8007000E)
    #define E_INVALIDARG    ((HRESULT)0x80070057)
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
// GUID
//

int32_t PAL_IsEqualGUID(GUID const*, GUID const*);

int32_t PAL_StringFromGUID2(GUID const*, LPOLESTR, int32_t);
HRESULT PAL_IIDFromString(LPCOLESTR, IID*);