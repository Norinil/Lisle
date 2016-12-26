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
#include <lisle/retex.h>

using namespace lisle;

TEST (Retex, throw_destroy_locked)
{
	// Ensure a permission exception is thrown when trying to destroy a retex that is locked
	EXPECT_THROW({retex retex; retex.lock();}, permission);
}

TEST (Retex, nothrow_destroy_unlocked)
{
	// Ensure nothing is thrown when trying to destroy a retex that is unlocked
	EXPECT_NO_THROW({retex retex; retex.lock(); retex.unlock();});
}

TEST (Retex, throw_unlock_unlocked)
{
	// Ensure a permission exception is thrown when trying to unlock a retex that is not locked
	try
	{
		retex retex;
		EXPECT_THROW(retex.unlock(), permission);
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (Retex, nothrow_deadlock)
{
	try
	{
		retex retex;
		ASSERT_NO_THROW(retex.lock());
		ASSERT_NO_THROW(retex.lock());
		ASSERT_NO_THROW(retex.unlock()); // else retex destruction would throw a permission exception
		ASSERT_NO_THROW(retex.unlock()); // else retex destruction would throw a permission exception
	}
	catch (...)
	{
		FAIL();
	}
}
