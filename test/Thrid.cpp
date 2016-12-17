/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2012, Markus Schütz
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
#include <gtest/gtest.h>
#include <lisle/Thrid>
#include <lisle/create>
#include <lisle/self>
#include <string>

using namespace lisle;
using namespace std;

void cancelable ()
{
	// Start that one joinable-suspended, then cancel it, then resume it, then join it.
}
TEST (ThridTest, cancel)
{
	// Ensure a thread started in suspended mode can be canceled and joined
	Thrid thread;
	Exit exit;
	ASSERT_NO_THROW(thread = create(Thread(&cancelable, Thread::Suspended)));
	ASSERT_NO_THROW(thread.cancel());
	ASSERT_NO_THROW(thread.resume());
	ASSERT_NO_THROW(exit = thread.join());
	EXPECT_EQ(canceled, exit);
}

Handle<string> returner ()
{
	return Handle<string>(new string("greetings from returner"));
}
TEST (ThridTest, retval)
{
	Thrid thread;
	Exit exit;
	Handle<string> retval;
	ASSERT_NO_THROW(thread = create(Thread(&returner)));
	yield();
	ASSERT_NO_THROW(exit = thread.join(retval));
	EXPECT_EQ(terminated, exit);
	EXPECT_EQ(string("greetings from returner"), *retval);
}
