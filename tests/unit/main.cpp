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

#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
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
        std::fprintf(stderr, "FAILED %s: %s\n\tin %s:%u\n", __func__, #x, __FILE__, __LINE__); \
    } \
}

template<typename E, size_t N>
constexpr size_t array_size(E const (&)[N])
{
    return N;
}

template<typename E, size_t N>
constexpr size_t array_size_bytes(E const (&)[N])
{
    return N * sizeof(E);
}

void test_memory()
{
    LPVOID p;
    {
        p = PAL_CoTaskMemAlloc(0);
        TEST_ASSERT(p != nullptr);
        PAL_CoTaskMemFree(p);
    }
    {
        p = PAL_CoTaskMemAlloc(1);
        TEST_ASSERT(p != nullptr);
        PAL_CoTaskMemFree(p);
    }
    {
        p = PAL_CoTaskMemAlloc(12);
        TEST_ASSERT(p != nullptr);
        PAL_CoTaskMemFree(p);
    }
    {
        // Check overflow, let OS handle other out of memory values.
        SIZE_T will_overflow = ~((SIZE_T)0) - 6;
        p = PAL_CoTaskMemAlloc(will_overflow);
        TEST_ASSERT(p == nullptr);
    }
}

void test_bstr()
{
    BSTR bstr;
    {
        WCHAR const expected[] = W("STRING");
        size_t expected_len = array_size(expected) - 1; // -1 for null
        size_t expected_byte_len = expected_len * sizeof(OLECHAR);

        bstr = PAL_SysAllocString(expected);
        TEST_ASSERT(0 == std::memcmp(expected, bstr, array_size_bytes(expected)));
        TEST_ASSERT(expected_len == PAL_SysStringLen(bstr));
        TEST_ASSERT(expected_byte_len == PAL_SysStringByteLen(bstr));
        PAL_SysFreeString(bstr);
    }
    {
        char const expected[] = "string";
        size_t expected_byte_len = array_size(expected) - 1; // -1 for null
        size_t expected_len = expected_byte_len / sizeof(OLECHAR);

        bstr = PAL_SysAllocStringByteLen(expected, expected_byte_len);
        TEST_ASSERT(0 == std::memcmp(expected, bstr, array_size_bytes(expected)));
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
        hr = PAL_IIDFromString(nullptr, &result);
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
        OLECHAR buffer[array_size(str_guid)];
        count = PAL_StringFromGUID2(&guid, buffer, array_size(buffer));
        TEST_ASSERT(count == array_size(buffer));

        // Convert from string back into GUID.
        hr = PAL_IIDFromString(buffer, &result);
        TEST_ASSERT(hr == S_OK);
        TEST_ASSERT(PAL_IsEqualGUID(&guid, &result));

        // Convert back into GUID.
        OLECHAR buffer2[array_size(str_guid)];
        count = PAL_StringFromGUID2(&result, buffer2, array_size(buffer2));
        TEST_ASSERT(count == array_size(buffer2));
        TEST_ASSERT(0 == std::memcmp(buffer, buffer2, array_size_bytes(buffer)));
    }
    {
        count = PAL_StringFromGUID2(&guid, nullptr, array_size(str_guid) - 1);
        TEST_ASSERT(count == 0);
    }
}

void test_interfaces()
{
    {
        REFIID iid = __uuidof(IUnknown);
        TEST_ASSERT(PAL_IsEqualGUID(&iid, &IID_IUnknown));
    }
    {
        REFIID iid = __uuidof(IClassFactory);
        TEST_ASSERT(PAL_IsEqualGUID(&iid, &IID_IClassFactory));
    }
}

int main()
{
    test_memory();
    test_bstr();
    test_guid();
    test_interfaces();

    std::printf("Test pass: %zd / %zd\n", test_count - test_failure, test_count);
    return test_failure == 0
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}