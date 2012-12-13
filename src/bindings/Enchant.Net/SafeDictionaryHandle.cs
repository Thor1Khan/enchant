/* Copyright (c) 2007 Eric Scott Albright
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

using System;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;

namespace Enchant
{
	internal sealed class SafeDictionaryHandle : SafeHandle
	{
		private readonly SafeBrokerHandle _broker;

		public SafeDictionaryHandle(SafeBrokerHandle broker, IntPtr handle)
			: base(IntPtr.Zero, true)
		{
			_broker = broker;
			SetHandle(handle);
		}


		///<summary>
		///When overridden in a derived class, gets a value indicating whether the handle value is invalid.
		///</summary>
		///<returns>
		///true if the handle is valid; otherwise, false.
		///</returns>
		public override bool IsInvalid
		{
			get { return (handle == IntPtr.Zero); }
		}

		///<summary>
		///When overridden in a derived class, executes the code required to free the handle.
		///</summary>
		///<returns>
		///true if the handle is released successfully; otherwise, in the event of a catastrophic failure, false. In this case, it generates a ReleaseHandleFailed Managed Debugging Assistant.
		///</returns>
		[ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
		protected override bool ReleaseHandle()
		{
			// The finalizer may finalize the broker and the dictionaries in any order
			// leading to a case where the broker may have already been released and thus
			// already cleaned up all its owned dictionaries. If that is the case, we don't
			// need to clean up.
			if (_broker.IsClosed)
				return true;
			Bindings.enchant_broker_free_dict(_broker, handle);
			return true;
		}
	}
}