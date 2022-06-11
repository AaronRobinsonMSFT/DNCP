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

#ifdef _MSC_VER
    #include <Windows.h>
    #include <wtypes.h>
#endif

#include <dncp.h>

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

template<size_t N>
constexpr size_t string_length(WCHAR const (&)[N])
{
    return N - 1; // -1 for null
}

template<size_t N>
constexpr size_t string_length(char const (&)[N])
{
    return N - 1; // -1 for null
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
    {
        dncp::cotaskmem_ptr<void> smart_ptr1{ nullptr };
        dncp::cotaskmem_ptr<int> smart_ptr2{ (int*)PAL_CoTaskMemAlloc(16) };
    }
}

void test_strings()
{
    // PAL_wcslen
    {
        WCHAR const len_0[] = W("");
        WCHAR const len_1[] = W("1");
        WCHAR const len_2[] = W("12");
        WCHAR const len_9[] = W("123456789");
        WCHAR const embedded_null[] = { W('1'), W('2'), W('\0'), W('4'), W('\0') };

        TEST_ASSERT(PAL_wcslen(len_0) == string_length(len_0));
        TEST_ASSERT(PAL_wcslen(len_1) == string_length(len_1));
        TEST_ASSERT(PAL_wcslen(len_2) == string_length(len_2));
        TEST_ASSERT(PAL_wcslen(len_9) == string_length(len_9));
        TEST_ASSERT(PAL_wcslen(embedded_null) == 2 && string_length(embedded_null) == 4);
    }

    // PAL_wcscmp
    {
        TEST_ASSERT(PAL_wcscmp(W(""), W("")) == 0);

        WCHAR const aStr1[] = W("a");
        WCHAR const aStr2[] = W("a");
        WCHAR const AStr[] = W("A");
        WCHAR const ZStr[] = W("Z");
        TEST_ASSERT(PAL_wcscmp(aStr1, aStr2) == 0);
        TEST_ASSERT(PAL_wcscmp(W(""), aStr1) == -1);
        TEST_ASSERT(PAL_wcscmp(aStr1, W("")) == 1);
        TEST_ASSERT(PAL_wcscmp(AStr, aStr1) == -1);
        TEST_ASSERT(PAL_wcscmp(aStr1, AStr) == 1);
        TEST_ASSERT(PAL_wcscmp(AStr, ZStr) == -1);
        TEST_ASSERT(PAL_wcscmp(ZStr, AStr) == 1);

        WCHAR const mixedStr1[] = W("aMqU");
        WCHAR const mixedStr2[] = W("aMqu");
        WCHAR const mixedStr3[] = W("aMq");
        TEST_ASSERT(PAL_wcscmp(mixedStr1, mixedStr2) == -1);
        TEST_ASSERT(PAL_wcscmp(mixedStr2, mixedStr1) == 1);
        TEST_ASSERT(PAL_wcscmp(mixedStr3, mixedStr1) == -1);
        TEST_ASSERT(PAL_wcscmp(mixedStr1, mixedStr3) == 1);
    }

    // PAL_wcsstr
    {
        WCHAR const find1[] = W("");
        WCHAR const find2[] = W("5");
        WCHAR const find3[] = W("5a");
        WCHAR const find4[] = W("67890");
        WCHAR const find5[] = W("1234567890");
        WCHAR const find6[] = W("12345678901234567890");
        WCHAR const str1[] = W("");
        WCHAR const str2[] = W("1234567890");
        TEST_ASSERT(PAL_wcsstr(str1, find1) == str1);
        TEST_ASSERT(PAL_wcsstr(str1, find2) == NULL);
        TEST_ASSERT(PAL_wcsstr(str2, find1) == str2);
        TEST_ASSERT(PAL_wcsstr(str2, find2) == (str2 + 4));
        TEST_ASSERT(PAL_wcsstr(str2, find3) == NULL);
        TEST_ASSERT(PAL_wcsstr(str2, find4) == (str2 + 5));
        TEST_ASSERT(PAL_wcsstr(str2, find5) == str2);
        TEST_ASSERT(PAL_wcsstr(str2, find6) == NULL);
    }
}

void test_bstr()
{
    BSTR bstr;
    {
        WCHAR const expected[] = W("STRING");
        size_t expected_len = string_length(expected);
        size_t expected_byte_len = expected_len * sizeof(OLECHAR);

        bstr = PAL_SysAllocString(expected);
        TEST_ASSERT(0 == std::memcmp(expected, bstr, array_size_bytes(expected)));
        TEST_ASSERT(expected_len == PAL_SysStringLen(bstr));
        TEST_ASSERT(expected_byte_len == PAL_SysStringByteLen(bstr));
        PAL_SysFreeString(bstr);
    }
    {
        char const expected[] = "string";
        size_t expected_byte_len = string_length(expected);
        size_t expected_len = expected_byte_len / sizeof(OLECHAR);

        bstr = PAL_SysAllocStringByteLen(expected, (UINT)expected_byte_len);
        TEST_ASSERT(0 == std::memcmp(expected, bstr, array_size_bytes(expected)));
        TEST_ASSERT(expected_byte_len == PAL_SysStringByteLen(bstr));
        TEST_ASSERT(expected_len == PAL_SysStringLen(bstr));
        PAL_SysFreeString(bstr);
    }
    {
        dncp::bstr_ptr smart_ptr1{ nullptr };
        dncp::bstr_ptr smart_ptr2{ PAL_SysAllocString(W("abcdefghijklmnopqrstuvwxyz")) };
    }
}

void test_guids()
{
    HRESULT hr;
    int32_t count;
    GUID result;
    GUID guid = { 0x12345678, 0x9abc, 0xdef0, { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 } };

    // Mixed case to validate parsing works.
    WCHAR const str_guid[] = W("{12345678-9abc-def0-1234-56789ABCDEF0}");

    {
        hr = PAL_IIDFromString(str_guid, &result);
        TEST_ASSERT(hr == S_OK);
        TEST_ASSERT(PAL_IsEqualGUID(&guid, &result));
    }
    {
        hr = PAL_IIDFromString(nullptr, &result);
        TEST_ASSERT(hr == S_OK);
        TEST_ASSERT(PAL_IsEqualGUID(&GUID_NULL, &result));
    }
    {
        TEST_ASSERT(E_INVALIDARG == PAL_IIDFromString(W("{123456789abc-def0-1234-56789ABCDEF0}"), &result));
        TEST_ASSERT(E_INVALIDARG == PAL_IIDFromString(W("{12345678-9abcdef0-1234-56789ABCDEF0}"), &result));
        TEST_ASSERT(E_INVALIDARG == PAL_IIDFromString(W("{12345678-9abc-def01234-56789ABCDEF0}"), &result));
        TEST_ASSERT(E_INVALIDARG == PAL_IIDFromString(W("{12345678-9abc-def0-123456789ABCDEF0}"), &result));
        TEST_ASSERT(E_INVALIDARG == PAL_IIDFromString(W("{12345678-9abc-def0-1234-56789ABCDEF0"), &result));
        TEST_ASSERT(E_INVALIDARG == PAL_IIDFromString(W("12345678-9abc-def0-1234-56789ABCDEF0}"), &result));
        TEST_ASSERT(E_INVALIDARG == PAL_IIDFromString(W("{12345678-9abc-def0-1234-56789ABCDEF0} "), &result));
    }
    {
        OLECHAR buffer[array_size(str_guid)];
        count = PAL_StringFromGUID2(&guid, buffer, (int32_t)array_size(buffer));
        TEST_ASSERT(count == array_size(buffer));

        // Convert from string back into GUID.
        hr = PAL_IIDFromString(buffer, &result);
        TEST_ASSERT(hr == S_OK);
        TEST_ASSERT(PAL_IsEqualGUID(&guid, &result));

        // Convert back into GUID.
        OLECHAR buffer2[array_size(str_guid)];
        count = PAL_StringFromGUID2(&result, buffer2, (int32_t)array_size(buffer2));
        TEST_ASSERT(count == array_size(buffer2));
        TEST_ASSERT(0 == std::memcmp(buffer, buffer2, array_size_bytes(buffer)));
    }
    {
        count = PAL_StringFromGUID2(&guid, nullptr, (int32_t)array_size(str_guid) - 1);
        TEST_ASSERT(count == 0);
    }
    {
        result = GUID_NULL;
        hr = PAL_CoCreateGuid(&result);
        TEST_ASSERT(hr == S_OK);
        TEST_ASSERT(0 != std::memcmp(&result, &GUID_NULL, sizeof(result)));

        // Confirm RFC-4122 version 4 random GUID.
        const uint16_t mask1   = 0xf000; // b1111000000000000
        const uint16_t version = 0x4000; // b0100000000000000
        TEST_ASSERT((result.Data3 & mask1) == version);

        const uint8_t mask2 = 0xc0; // b11000000
        const uint8_t value = 0x80; // b10000000
        TEST_ASSERT((result.Data4[0] & mask2) == value);
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

using dncp::com_ptr;

void test_com_ptr()
{
    struct : public IUnknown
    {
    public:
        ULONG RefCount;

    public: // IUnknown
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void **)
        {
            return E_NOTIMPL;
        }

        virtual ULONG STDMETHODCALLTYPE AddRef( void)
        {
            return RefCount++;
        }

        virtual ULONG STDMETHODCALLTYPE Release( void)
        {
            return RefCount--;
        }
    } test_ptr{};

    {
        test_ptr.RefCount = 0;
        {
            com_ptr<IUnknown> t{ &test_ptr };
            TEST_ASSERT(test_ptr.RefCount == 1);
        }
        TEST_ASSERT(test_ptr.RefCount == 0);
    }
    {
        test_ptr.RefCount = 0;
        com_ptr<IUnknown> t{ &test_ptr };
        t.Release();
        TEST_ASSERT(test_ptr.RefCount == 0 && t.p == nullptr);
    }
    {
        test_ptr.RefCount = 0;
        com_ptr<IUnknown> t{};
        TEST_ASSERT(t.p == nullptr);
        t.Attach(&test_ptr);
        TEST_ASSERT(test_ptr.RefCount == 0 && t.p != nullptr);
        TEST_ASSERT(&test_ptr == t.Detach());
        TEST_ASSERT(test_ptr.RefCount == 0 && t.p == nullptr);
    }
    {
        com_ptr<IUnknown> t{ &test_ptr };
        t->QueryInterface(GUID_NULL, nullptr);
        t->AddRef();
        t->Release();
    }
}

int main()
{
    test_memory();
    test_strings();
    test_bstr();
    test_guids();
    test_interfaces();
    test_com_ptr();

    std::printf("Test pass: %zd / %zd\n", test_count - test_failure, test_count);
    return test_failure == 0
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}