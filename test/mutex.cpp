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
#include <lisle/mutex.h>

using namespace lisle;

TEST (Mutex, throw_destroy_locked)
{
	// Ensure a permission exception is thrown when trying to destroy a mutex that is locked
	EXPECT_THROW({mutex mutex; mutex.lock();}, permission);
}

TEST (Mutex, nothrow_destroy_unlocked)
{
	// Ensure nothing is thrown when trying to destroy a mutex that is unlocked
	EXPECT_NO_THROW({mutex mutex; mutex.lock(); mutex.unlock();});
}

TEST (Mutex, throw_unlock_unlocked)
{
	// Ensure a permission exception is thrown when trying to unlock a mutex that is not locked
	try
	{
		mutex mutex;
		EXPECT_THROW(mutex.unlock(), permission);
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (Mutex, throw_deadlock)
{
	try
	{
		mutex mutex;
		ASSERT_NO_THROW(mutex.lock());
		ASSERT_THROW(mutex.lock(), deadlock);
		ASSERT_NO_THROW(mutex.unlock()); // else mutex destruction would throw a permission exception
	}
	catch (...)
	{
		FAIL();
	}
}
