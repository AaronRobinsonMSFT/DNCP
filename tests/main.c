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
        fprintf(stderr, "FAILED %s: %s\n\tin %s @ %u\n", __func__, #x, __FILE__, __LINE__); \
    } \
}

void test_memory();
void test_guid();

int main()
{
    test_memory();

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
        p = PAL_CoTaskMemAlloc(12);
        TEST_ASSERT(p != NULL);
        PAL_CoTaskMemFree(p);
    }
}

void test_guid()
{
}