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

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <dncp.h>

//
// The BSTR is an allocation that is at least 6-bytes in size.
// The layout of the type is a pointer preceeded by 4 bytes
// that contain length, in bytes, of the contents.
// For example, on a little-endian platform a BSTR that is allocated
// using LPOLESTR("a"), would be in memory as:
//
//  | 2 | 0 | 0 | 0 | 'a' | '\0' | '\0' | '\0' |
//                  ^ Where the BSTR would point.
//
// We do want to ensure we are 8-byte aligned. CoTaskMemAlloc
// provides this guarantee, but with the offset of 4-bytes for
// byte length this means our pointer will be observed to be only
// 4-byte aligned. We compensate for this by allocating a pointer
// size of extra memory for the embedded byte size.

#define BSTR_MIN_ALLOC (sizeof(SIZE_T) + sizeof(OLECHAR))

static bool ComputeAllocSize(SIZE_T elemCount, SIZE_T elemSize, SIZE_T* bytesToAlloc)
{
    SIZE_T allocMaybe = elemCount * elemSize;
    if (elemSize != 0
        && (allocMaybe / elemSize) != elemCount)
    {
        return false;
    }

    SIZE_T alloc = allocMaybe + BSTR_MIN_ALLOC;
    if (alloc < allocMaybe)
        return false;

    *bytesToAlloc = alloc;
    return true;
}

static void* AllocAlignedBstr(SIZE_T byteAlloc)
{
    char* alloc = (char*)PAL_CoTaskMemAlloc(byteAlloc);
    if (alloc == NULL)
        return NULL;

    if (sizeof(SIZE_T) == 8)
    {
        *(UINT*)alloc = 0;
        alloc = alloc + 4;
    }

    return alloc;
}

static void FreeAlignedBstr(void* alloc)
{
    assert(alloc != NULL);
    if (sizeof(SIZE_T) == 8)
        alloc = (char*)alloc - 4;

    PAL_CoTaskMemFree(alloc);
}

static UINT OLEStrLen(LPCOLESTR str)
{
    assert(sizeof(str[0]) == sizeof(WCHAR));
    size_t len = PAL_wcslen(str);
    assert(len <= UINT_MAX);
    return (UINT)len;
}

BSTR PAL_SysAllocString(LPCOLESTR str)
{
    if (str == NULL)
      return NULL;

    return PAL_SysAllocStringLen(str, OLEStrLen(str));
}

BSTR PAL_SysAllocStringLen(LPCOLESTR str, UINT len)
{
    BSTR bstr;
    SIZE_T byteAlloc = 0;
    if (!ComputeAllocSize(len, sizeof(str[0]), &byteAlloc))
        return NULL;

    bstr = (LPOLESTR)AllocAlignedBstr(byteAlloc);
    if (bstr == NULL)
        return NULL;

    // Set the content length.
    *(UINT*)bstr = len * sizeof(str[0]);

    // Increment past content length.
    bstr = (BSTR)((char*)bstr + sizeof(UINT));

    if(str != NULL)
        memcpy(bstr, str, len * sizeof(str[0]));

    bstr[len] = W('\0'); //Ensure null termination
    return bstr;
}

BSTR PAL_SysAllocStringByteLen(char const* str, UINT len)
{
    BSTR bstr;
    SIZE_T byteAlloc = 0;
    if (!ComputeAllocSize(len, sizeof(str[0]), &byteAlloc))
        return NULL;

    bstr = (LPOLESTR)AllocAlignedBstr(byteAlloc);
    if (bstr == NULL)
        return NULL;

    // Set the content length.
    *(UINT*)bstr = len * sizeof(str[0]);

    // Increment past content length.
    bstr = (BSTR)((char*)bstr + sizeof(UINT));

    if(str != NULL)
        memcpy(bstr, str, len * sizeof(str[0]));

    //Ensure null termination for the single byte type.
    *((char*)bstr + len) = '\0';
    *(OLECHAR*)((char*)bstr + ((len + 1) & ~1)) = W('\0');
    return bstr;
}

void PAL_SysFreeString(BSTR str)
{
    if (str == NULL)
      return;

    FreeAlignedBstr((char*)str - sizeof(UINT));
}

UINT PAL_SysStringLen(BSTR str)
{
    if(str == NULL)
      return 0;

    return (UINT)(((UINT*)str)[-1]) / sizeof(OLECHAR);
}

UINT PAL_SysStringByteLen(BSTR str)
{
    if(str == NULL)
      return 0;

    return (UINT)(((UINT*)str)[-1]);
}