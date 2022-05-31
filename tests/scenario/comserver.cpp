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

#include <cassert>
#include <cstdint>
#include <atomic>
#include <new>
#include <array>

#ifdef _MSC_VER
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <Objbase.h>
    #include <combaseapi.h>
#endif

#include <dncompal.h>

// 4A23EA71-363E-4609-AD85-CFBF5716A9FA
static IID const IID_IComServer = { 0x4A23EA71, 0x363E, 0x4609, { 0xAD, 0x85, 0xCF, 0xBF, 0x57, 0x16, 0xA9, 0xFA } };

struct IComServer : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GuidToString(
        REFGUID guid,
        BSTR *guidAsString) PURE;
};

class ComServer final : IComServer
{
    std::atomic<ULONG> _refCount;

public:
    ComServer()
        : _refCount{1}
    { }

public: // IComServer
    virtual HRESULT STDMETHODCALLTYPE GuidToString(
        REFGUID guid,
        BSTR *guidAsString)
    {
        if (guidAsString == nullptr)
            return E_POINTER;

        std::array<WCHAR, 64> buffer;
        if (0 == PAL_StringFromGUID2(&guid, buffer.data(), (int32_t)buffer.size()))
            return E_FAIL;

        BSTR bstr = PAL_SysAllocString(buffer.data());
        if (bstr == nullptr)
            return E_OUTOFMEMORY;

        *guidAsString = bstr;
        return S_OK;
    }

public: // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        void **ppvObject)
    {
        if (ppvObject == nullptr)
            return E_POINTER;

        if (riid == IID_IUnknown)
        {
            *ppvObject = static_cast<IUnknown*>(this);
        }
        else if (riid == IID_IComServer)
        {
            *ppvObject = static_cast<IComServer*>(this);
        }
        else
        {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        (void)AddRef();
        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void)
    {
        return ++_refCount;
    }

    virtual ULONG STDMETHODCALLTYPE Release( void)
    {
        ULONG count = --_refCount;
        if (count == 0)
            delete this;
        return count;
    }
};

EXTERN_C HRESULT GetComServer(REFIID riid, LPVOID *ppv)
{
    if (ppv == nullptr)
        return E_POINTER;

    ComServer* server = new (std::nothrow) ComServer();
    if (server == nullptr)
        return E_OUTOFMEMORY;

    HRESULT hr = server->QueryInterface(riid, ppv);
    if (FAILED(hr))
    {
        (void)server->Release();
        return hr;
    }

    return S_OK;
}