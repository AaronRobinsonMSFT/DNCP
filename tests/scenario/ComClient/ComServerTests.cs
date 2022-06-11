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
using System.Runtime.InteropServices;

using Xunit;

namespace ComClient
{
    [Guid("4A23EA71-363E-4609-AD85-CFBF5716A9FA")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IComServer
    {
        [return: MarshalAs(UnmanagedType.BStr)]
        string GuidToString(in Guid guid);

        [return: MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)]
        int[] DoubleIntegers(int length, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] integers);
    }

    public static unsafe class ComServer
    {
        [DllImport("comserver", PreserveSig = false)]
        public static extern void CreateComServer(Guid* iid, IntPtr* server);

        public static void TestGuidToString(IComServer server)
        {
            var guid = Guid.NewGuid();
            string asStr = server.GuidToString(in guid);

            // The COM APIs being used convert GUIDs to uppercase.
            Assert.Equal(guid.ToString("B").ToUpper(), asStr);
        }

        public static void TestDoubleIntegers(IComServer server)
        {
            Assert.Throws<NullReferenceException>(() => server.DoubleIntegers(1, null!));

            int[] ints = new[] { 1, 2, 3, 4, 5 };
            int[] result = server.DoubleIntegers(ints.Length, ints);

            for (int i = 0; i < ints.Length; ++i)
            {
                Assert.Equal(ints[i] * 2, result[i]);
            }
        }
    }
}