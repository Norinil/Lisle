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
#include <lisle/Semaphore>
#include <lisle/Strid>

using namespace lisle;
using namespace std;

#ifdef _MSC_VER
#pragma warning (disable:4512)
#endif

/// Test that semaphore post throws an overflow exception if it reaches max
TEST (SemaphoreTest, max_overflow)
{
	Semaphore semaphore(Semaphore::max());
	ASSERT_THROW(semaphore.post(), overflow);
}

/// Test that semaphore post doesn't throw in normal conditions
TEST (SemaphoreTest, nooverflow)
{
	Semaphore semaphore(Semaphore::max()-1);
	ASSERT_NO_THROW(semaphore.post());
}

/// Test that semaphore wait is postable
TEST (SemaphoreTest, wait_post)
{
	Semaphore sem;
	class Thread : public Strand
	{
	public:
		bool done;
		Thread (Semaphore& sem) : done(false), sem(sem) {}
	private:
		Semaphore& sem;
		void main ()
		{
			sem.wait();
			done = true;
		}
	};
	Strid handle = Strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	sem.post();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
}

/// Test that semaphore trywait is postable
TEST (SemaphoreTest, trywait_post)
{
	Semaphore sem;
	class Thread : public Strand
	{
	public:
		bool done;
		int val;
		Thread (Semaphore& sem) : done(false), val(-1), sem(sem) {}
	private:
		Semaphore& sem;
		void main ()
		{
			if (sem.trywait())
				val = 1;
			else
				val = 0;
			done = true;
		}
	};
	Strid handle = Strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	sem.post();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
	EXPECT_NE(thread->val, -1);
}

/// Test that semaphore wait is cancelable
TEST (SemaphoreTest, wait_cancel)
{
	Semaphore sem;
	class Thread : public Strand
	{
	public:
		bool done;
		Thread (Semaphore& sem) : done(false), sem(sem) {}
	private:
		Semaphore& sem;
		void main ()
		{
			sem.wait();
			// never reached if thread was canceled
			done = true;
		}
	};
	Strid handle = Strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	sleep(Duration(0.01)); // give the thread a chance to wait
	handle.cancel();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
}
