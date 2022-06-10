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

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <dncp.h>

size_t PAL_wcslen(WCHAR const* str)
{
    assert(str != NULL);

    size_t len = 0;
    while (*str++ != W('\0'))
        len++;
    return len;
}

int PAL_wcscmp(WCHAR const* str1, WCHAR const* str2)
{
    assert(str1 != NULL && str2 != NULL);

    int i = 0;
    while (str1[i] == str2[i] && str1[i] != W('\0'))
        ++i;

    int res = (int)str1[i] - str2[i];
    return ((-res) < 0) - (res < 0);
}

WCHAR* PAL_wcsstr(WCHAR const* dest, WCHAR const* src)
{
    assert(dest != NULL && src != NULL);

    if (src[0] == W('\0'))
        return (WCHAR*)dest;

    int i;
    WCHAR* res = NULL;
    while (*dest != W('\0'))
    {
        for (i = 0; true; ++i)
        {
            if (src[i] == W('\0'))
            {
                res = (WCHAR*)dest;
                goto exit;
            }
            else if (dest[i] == W('\0'))
            {
                res = NULL;
                goto exit;
            }
            else if (dest[i] != src[i])
            {
                break;
            }
        }

        dest++;
    }

exit:
    return res;
}