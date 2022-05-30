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

using System;
using System.Collections;
using System.Runtime.InteropServices;

using Xunit;

namespace ComClient;

[Guid("4A23EA71-363E-4609-AD85-CFBF5716A9FA")]
public interface IComServer
{
    void GuidToString(in Guid guid, [MarshalAs(UnmanagedType.BStr)] out string guidAsString);
}

public unsafe class UseComWrappers
{
    [DllImport("comserver", PreserveSig = false)]
    static extern void GetComServer(Guid* iid, IntPtr* server);

    [Fact]
    public void InvalidArguments()
    {
        Assert.Throws<NullReferenceException>(() => GetComServer(null, null));
    }

    [Fact]
    public void CallServer()
    {
        ComWrappersImpl cw = new();

        var iid = typeof(IComServer).GUID;
        IntPtr serverPtr;
        GetComServer(&iid, &serverPtr);

        IComServer server;
        try
        {
            server = (IComServer)cw.GetOrCreateObjectForComInstance(serverPtr, CreateObjectFlags.None);
        }
        finally
        {
            Marshal.Release(serverPtr);
        }

        var guid = Guid.NewGuid();
        server.GuidToString(in guid, out string asStr);
        Assert.Equal(guid.ToString("B"), asStr);
    }

    private unsafe class ComWrappersImpl : ComWrappers
    {
        protected override unsafe ComInterfaceEntry* ComputeVtables(object obj, CreateComInterfaceFlags flags, out int count)
            => throw new NotImplementedException();

        protected override object CreateObject(IntPtr externalComObject, CreateObjectFlags flags)
        {
            var iid = typeof(IComServer).GUID;
            IntPtr comServer;
            int hr = Marshal.QueryInterface(externalComObject, ref iid, out comServer);
            Assert.Equal(0, hr);
            return new IComServerWrapper(comServer);
        }

        protected override void ReleaseObjects(IEnumerable objects)
            => throw new NotImplementedException();

        public class IComServerWrapper : IComServer
        {
            [StructLayout(LayoutKind.Sequential)]
            private struct VtblPtr
            {
                public void* Vtbl;
            }

            [StructLayout(LayoutKind.Sequential)]
            private struct IComServerWrapperVtbl
            {
                public void* QueryInterface;
                public delegate* unmanaged<void*, int> AddRef;
                public delegate* unmanaged<void*, int> Release;
                public delegate* unmanaged<void*, Guid*, void**, int> GuidToString;
            }

            private readonly VtblPtr* instancePtr;
            private readonly IComServerWrapperVtbl* vtable;

            public IComServerWrapper(IntPtr instancePtr)
            {
                this.instancePtr = (VtblPtr*)instancePtr;
                this.vtable = (IComServerWrapperVtbl*)this.instancePtr->Vtbl;
            }

            ~IComServerWrapper()
            {
                if (this.instancePtr != null)
                    this.vtable->Release(this.instancePtr);
            }

            public void GuidToString(in Guid guid, [MarshalAs(UnmanagedType.BStr)] out string guidAsString)
            {
                int hr;
                void* str = null;
                fixed (Guid* g = &guid)
                {
                    hr = this.vtable->GuidToString(this.instancePtr, g, &str);
                    if (hr < 0)
                        Marshal.ThrowExceptionForHR(hr);
                }

                guidAsString = Marshal.PtrToStringBSTR((IntPtr)str);
            }
        }
    }
}