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
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dncompal.h>

static size_t test_count = 0;
static size_t test_failure = 0;

#define TEST_ASSERT(x) \
{ \
    test_count++; \
    bool res = (x); \
    if (!res) \
    { \
        test_failure++; \
        fprintf(stderr, "FAILED %s: %s\n\tin %s:%u\n", __func__, #x, __FILE__, __LINE__); \
    } \
}

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

void test_memory();
void test_bstr();
void test_guid();

int main()
{
    test_memory();
    test_bstr();
    test_guid();

    printf("Test pass: %zd / %zd\n", test_count - test_failure, test_count);
    return test_failure == 0
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}

void test_memory()
{
    LPVOID p;
    {
        p = PAL_CoTaskMemAlloc(0);
        TEST_ASSERT(p != NULL);
        PAL_CoTaskMemFree(p);
    }
    {
        p = PAL_CoTaskMemAlloc(1);
        TEST_ASSERT(p != NULL);
        PAL_CoTaskMemFree(p);
    }
    {
        p = PAL_CoTaskMemAlloc(12);
        TEST_ASSERT(p != NULL);
        PAL_CoTaskMemFree(p);
    }
    {
        // Check overflow, let OS handle other out of memory values.
        SIZE_T will_overflow = ~((SIZE_T)0) - 6;
        p = PAL_CoTaskMemAlloc(will_overflow);
        TEST_ASSERT(p == NULL);
    }
}

void test_bstr()
{
    BSTR bstr;
    {
        WCHAR const expected[] = W("STRING");
        size_t expected_len = ARRAY_SIZE(expected) - 1; // -1 for null
        size_t expected_byte_len = expected_len * sizeof(OLECHAR);

        bstr = PAL_SysAllocString(expected);
        TEST_ASSERT(0 == memcmp(expected, bstr, ARRAY_SIZE(expected)));
        TEST_ASSERT(expected_len == PAL_SysStringLen(bstr));
        TEST_ASSERT(expected_byte_len == PAL_SysStringByteLen(bstr));
        PAL_SysFreeString(bstr);
    }
    {
        char const expected[] = "string";
        size_t expected_byte_len = ARRAY_SIZE(expected) - 1; // -1 for null
        size_t expected_len = expected_byte_len / sizeof(OLECHAR);

        bstr = PAL_SysAllocStringByteLen(expected, expected_byte_len);
        TEST_ASSERT(0 == memcmp(expected, bstr, ARRAY_SIZE(expected)));
        TEST_ASSERT(expected_byte_len == PAL_SysStringByteLen(bstr));
        TEST_ASSERT(expected_len == PAL_SysStringLen(bstr));
        PAL_SysFreeString(bstr);
    }
}

void test_guid()
{
    HRESULT hr;
    int32_t count;
    GUID result;
    GUID guid = { 0x12345678, 0x9abc, 0xdef0, { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 } };
    GUID null = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };
    WCHAR const str_guid[] = W("{12345678-9abc-def0-1234-56789ABCDEF0}");

    {
        hr = PAL_IIDFromString(str_guid, &result);
        TEST_ASSERT(hr == S_OK);
        TEST_ASSERT(PAL_IsEqualGUID(&guid, &result));
    }
    {
        hr = PAL_IIDFromString(NULL, &result);
        TEST_ASSERT(hr == S_OK);
        TEST_ASSERT(PAL_IsEqualGUID(&null, &result));
    }
    {
        TEST_ASSERT(CO_E_CLASSSTRING == PAL_IIDFromString(W("{123456789abc-def0-1234-56789ABCDEF0}"), &result));
        TEST_ASSERT(CO_E_CLASSSTRING == PAL_IIDFromString(W("{12345678-9abcdef0-1234-56789ABCDEF0}"), &result));
        TEST_ASSERT(CO_E_CLASSSTRING == PAL_IIDFromString(W("{12345678-9abc-def01234-56789ABCDEF0}"), &result));
        TEST_ASSERT(CO_E_CLASSSTRING == PAL_IIDFromString(W("{12345678-9abc-def0-123456789ABCDEF0}"), &result));
        TEST_ASSERT(CO_E_CLASSSTRING == PAL_IIDFromString(W("{12345678-9abc-def0-1234-56789ABCDEF0"), &result));
        TEST_ASSERT(CO_E_CLASSSTRING == PAL_IIDFromString(W("12345678-9abc-def0-1234-56789ABCDEF0}"), &result));
        TEST_ASSERT(CO_E_CLASSSTRING == PAL_IIDFromString(W("{12345678-9abc-def0-1234-56789ABCDEF0} "), &result));
    }
    {
        OLECHAR buffer[ARRAY_SIZE(str_guid)];
        count = PAL_StringFromGUID2(&guid, buffer, ARRAY_SIZE(buffer));
        TEST_ASSERT(count == ARRAY_SIZE(buffer));
        TEST_ASSERT(0 == memcmp(buffer, str_guid, ARRAY_SIZE(buffer)));
    }
    {
        count = PAL_StringFromGUID2(&guid, NULL, ARRAY_SIZE(str_guid) - 1);
        TEST_ASSERT(count == 0);
    }
}