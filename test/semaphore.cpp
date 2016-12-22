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
#include <lisle/semaphore.h>
#include <lisle/strid.h>

using namespace lisle;
using namespace std;

#ifdef _MSC_VER
#pragma warning (disable:4512)
#endif

/// Test that semaphore post throws an overflow exception if it reaches max
TEST (Semaphore, max_overflow)
{
	semaphore sem(semaphore::max());
	ASSERT_THROW(sem.post(), overflow);
}

/// Test that semaphore post doesn't throw in normal conditions
TEST (Semaphore, nooverflow)
{
	semaphore sem(semaphore::max()-1);
	ASSERT_NO_THROW(sem.post());
}

/// Test that semaphore wait is postable
TEST (Semaphore, wait_post)
{
	semaphore sem;
	class Thread : public strand
	{
	public:
		bool done;
		Thread (semaphore& sem) : done(false), sem(sem) {}
	private:
		semaphore& sem;
		void main ()
		{
			sem.wait();
			done = true;
		}
	};
	strid handle = strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	sem.post();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
}

/// Test that semaphore trywait is postable
TEST (Semaphore, trywait_post)
{
	semaphore sem;
	class Thread : public strand
	{
	public:
		bool done;
		int val;
		Thread (semaphore& sem) : done(false), val(-1), sem(sem) {}
	private:
		semaphore& sem;
		void main ()
		{
			if (sem.trywait())
				val = 1;
			else
				val = 0;
			done = true;
		}
	};
	strid handle = strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	sem.post();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
	EXPECT_NE(thread->val, -1);
}

/// Test that semaphore wait is cancelable
TEST (Semaphore, wait_cancel)
{
	semaphore sem;
	class Thread : public strand
	{
	public:
		bool done;
		Thread (semaphore& sem) : done(false), sem(sem) {}
	private:
		semaphore& sem;
		void main ()
		{
			sem.wait();
			// never reached if thread was canceled
			done = true;
		}
	};
	strid handle = strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	sleep(duration(0.01)); // give the thread a chance to wait
	handle.cancel();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
}
