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
#include <dncp.h>

// CoTaskMemAlloc always aligns on an 8-byte boundary.
#define ALIGN 8

LPVOID PAL_CoTaskMemAlloc(SIZE_T cb)
{
    // Ensure malloc always allocates.
    if (cb == 0)
        cb = ALIGN;

    // Align the allocation size.
    SIZE_T cb_safe = (cb + (ALIGN - 1)) & ~(ALIGN - 1);
    if (cb_safe < cb) // Overflow
        return NULL;

    return aligned_alloc(ALIGN, cb_safe);
}

void PAL_CoTaskMemFree(LPVOID pv)
{
    free(pv);
}
