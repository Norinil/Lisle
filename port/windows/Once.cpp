/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2012, Markus Schütz
//
// This library is free software. You can redistribute  and/or  modify it under
// the terms of the GNU Lesser General Public License  as published by the Free
// Software Foundation.  Either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,  but WITHOUT
// ANY  WARRANTY.  Without  even  the  implied  warranty  of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received  a  copy  of  the GNU Lesser General Public License
// along with this library. If not, write to the Free Software Foundation, Inc,
// 59  Temple Place,  Suite 330,  Boston,  MA  02111-1307  USA,  or consult URL
// http://www.gnu.org/licenses/lgpl.html.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include "lockcmpxchg"
#include <lisle/Once>

namespace lisle {

Once::Once (void(* once)())
: once(once)
{
	control.done = false;
	control.todo = 1;
}

void Once::run ()
{
	/* The first thread that can decrement the control.todo counter to zero
	 * executes the 'once' function.
	 * Once the 'once' function is executed the control.done flag falls
	 * to false, thus disabling any other execution of 'once'.
	 * In the case another thread enters this function before the 'once' executor
	 * could set control.done to false it will not be able to decrement the
	 * control.todo counter, but since it may rely on stuff initialized in
	 * 'once', the thread is sent into a sleep loop until the control.done
	 * flag becomes false. Hopefully the sleep loop will not eat up too much CPU.
	 */
	if (!control.done)
	{
		if (InterlockedDecrement(&control.todo) == 0)
		{
			(*once)();
			control.done = true;
		}
		else
		{
			while (!control.done)
				Sleep(0);
		}
	}
}

}
