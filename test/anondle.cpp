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
#include "test.h"
#include <lisle/anondle.h>

using namespace lisle;

template <typename T> class Handle : public handle<T>
{
public:
	Handle (T* data) : base(data) {}
	countic* users () const { return base::users; }
private:
	typedef handle<T> base;
};

TEST (Anondle, ctor_handle)
{
	// Ensure the handle's users count is incremented
	Handle<double> handle(new double(3.1416));
	{
		anondle anondle(handle);
		// handle should now have 2 users
		ASSERT_FALSE(handle.users()->dectz());
		ASSERT_TRUE(handle.users()->dectz());
		// restore the 2 users count for handle
		ASSERT_NO_THROW(handle.users()->inc());
		ASSERT_NO_THROW(handle.users()->inc());
	}
	// handle should have 1 user
	ASSERT_TRUE(handle.users()->dectz());
	ASSERT_NO_THROW(handle.users()->inc());
}
