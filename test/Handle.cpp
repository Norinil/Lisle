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
#include <lisle/Handle>

using namespace lisle;

template <typename T> class THandle : public Handle<T>
{
public:
	THandle (T* data) : base(data) {}
	Countic* users () const { return base::users; }
private:
	typedef Handle<T> base;
};

TEST (HandleTest, ctorValue)
{
	// Ensure the users count initializes to 1
	THandle<double> handle(new double(3.1416));
	ASSERT_EQ(1, *handle.users());
	ASSERT_EQ(3.1416, *handle);
}

TEST (HandleTest, ctorCopy)
{
	// Ensure the users count is incremented when copying a handle
	THandle<double> handle(new double(3.1416));
	THandle<double> copy(handle);
	ASSERT_EQ(copy.users(), handle.users());
	ASSERT_EQ(2, *handle.users());
	ASSERT_EQ(2, *copy.users());
	ASSERT_EQ(3.1416, *handle);
	ASSERT_EQ(3.1416, *copy);
}

TEST (HandleTest, assignSelf)
{
	// Ensure that a self assignment doesn't increase users count
	THandle<double> handle(new double(3.1416));
	ASSERT_NO_THROW(handle = handle);
	ASSERT_EQ(1, *handle.users());
	ASSERT_EQ(3.1416, *handle);
}
