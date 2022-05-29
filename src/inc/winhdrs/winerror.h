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

#ifndef _WINHDRS_WINERROR_H_
#define _WINHDRS_WINERROR_H_

#define SEVERITY_SUCCESS    0
#define SEVERITY_ERROR      1
#define FACILITY_WIN32      7

#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((ULONG)(sev)<<31) | ((ULONG)(fac)<<16) | ((ULONG)(code))) )

#define SUCCEEDED(x) (x >= 0)
#define FAILED(x) (x < 0)

// Win32 HRESULTs
#define S_OK            ((HRESULT)0)
#define S_FALSE         ((HRESULT)1)
#define E_OUTOFMEMORY   ((HRESULT)0x8007000E)
#define E_INVALIDARG    ((HRESULT)0x80070057)

#define E_NOTIMPL        ((HRESULT)0x80004001)
#define E_NOINTERFACE    ((HRESULT)0x80004002)
#define E_POINTER        ((HRESULT)0x80004003)
#define E_ABORT          ((HRESULT)0x80004004)
#define E_FAIL           ((HRESULT)0x80004005)

#define CO_E_CLASSSTRING ((HRESULT)0x800401F3)

#define E_NOT_SET MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 1168)

#endif // _WINHDRS_WINERROR_H_
