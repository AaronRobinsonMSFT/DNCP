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

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dncompal.h>
#include <util.h>

BOOL PAL_IsEqualGUID(GUID const* g1, GUID const* g2)
{
    return !memcmp(g1, g2, sizeof(*g1)) ? TRUE : FALSE;
}

static int const uuid_str_size = ARRAY_SIZE("12345678-1234-1234-1234-123456789abc") - 1; // -1 for null
static int const guid_str_size = uuid_str_size + 2; // +2 for the surrounding braces

int32_t PAL_StringFromGUID2(GUID const* guid, LPOLESTR buffer, int32_t count)
{
    if (count <= guid_str_size)
        return 0;

    char local[guid_str_size + 1]; // +1 for null
    int res = snprintf(local, ARRAY_SIZE(local), "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
            guid->Data1, guid->Data2, guid->Data3,
            guid->Data4[0], guid->Data4[1],
            guid->Data4[2], guid->Data4[3],
            guid->Data4[4], guid->Data4[5],
            guid->Data4[6], guid->Data4[7]);
    if (res < 0)
        return res;

    // +1 for null.
    res += 1;

    // Widen by casting. This is okay because all characters
    // in a GUID are the same in both encodings.
    for (int i = 0; i < res; i++)
        buffer[i] = (WCHAR)local[i];

    return res;
}

// GUID contains braces
static bool GUIDFromString(LPCOLESTR str, GUID* guid);

// UUID lacks braces
static bool UUIDFromString(LPCOLESTR str, GUID* guid);

HRESULT PAL_IIDFromString(LPCOLESTR str, IID* iid)
{
    if (str == NULL)
    {
        (void)memset(iid, 0, sizeof(*iid));
        return S_OK;
    }

    return GUIDFromString(str, iid)
        ? S_OK
        : E_INVALIDARG;
}

static bool GUIDFromString(LPCOLESTR str, GUID* guid)
{
    if (*str++ != W('{'))
        return false;

    if (!UUIDFromString(str, guid))
        return false;

    str += uuid_str_size;

    if (*str++ != W('}'))
        return false;

    if (*str != W('\0'))
        return false;

    return true;
}

static uint32_t ParseHexToValue(LPCOLESTR* pstr, size_t sizeInBytes, WCHAR delim, bool* valid);

static bool UUIDFromString(LPCOLESTR str, GUID* guid)
{
    bool is_valid;
    guid->Data1 = ParseHexToValue(&str, sizeof(guid->Data1), W('-'), &is_valid);
    if (!is_valid)
        return false;
    guid->Data2 = (uint16_t)ParseHexToValue(&str, sizeof(guid->Data2), W('-'), &is_valid);
    if (!is_valid)
        return false;
    guid->Data3 = (uint16_t)ParseHexToValue(&str, sizeof(guid->Data3), W('-'), &is_valid);
    if (!is_valid)
        return false;

    // Parse the remainder of the string into the uint8_t[]. A '-' delimiter is
    // present after the first two bytes (i.e., index 1).
    for (int i = 0; i < ARRAY_SIZE(guid->Data4); ++i)
    {
        int delim = i == 1 ? W('-') : 0;
        guid->Data4[i] = (uint8_t)ParseHexToValue(&str, sizeof(*guid->Data4), delim, &is_valid);
        if (!is_valid)
            return false;
    }

    return true;
}

static uint32_t ParseHexToValue(LPCOLESTR* pstr, size_t sizeInBytes, WCHAR delim, bool* valid)
{
    assert(0 < sizeInBytes && sizeInBytes <= sizeof(uint32_t));
    *valid = true;

    // A single byte requires two hexadecimal values.
    uint32_t val;
    for (size_t count = 0; count < (sizeInBytes * 2); count++, (*pstr)++)
    {
        OLECHAR ch = (*pstr)[0];
        if (ch >= W('0') && ch <= W('9'))
        {
            val = (val << 4) + ch - W('0');
        }
        else if (ch >= W('A') && ch <= W('F'))
        {
            val = (val << 4) + ch - W('A') + 10;
        }
        else if (ch >= W('a') && ch <= W('f'))
        {
            val = (val << 4) + ch - W('a') + 10;
        }
        else
        {
            *valid = false;
            return 0;
        }
    }

    if (delim != 0)
    {
        OLECHAR ch = (*pstr)[0];
        (*pstr)++; // Consume character
        *valid = ch == delim;
    }

    return val;
}