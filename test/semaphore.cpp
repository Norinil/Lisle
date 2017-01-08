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

// Test that the default semaphore gets 1 resource and 1 available
TEST (Semaphore, ctor)
{
	semaphore sem;
	EXPECT_EQ(sem.available(), 1);
	ASSERT_EQ(sem.resources(), 1);
}

// Test that the constructor throws underflow in case of construction with 0
TEST (Semaphore, ctor_0_throws_underflow)
{
	ASSERT_THROW(semaphore sem(0), underflow);
}

// Test that the semaphore get the specified resources and available
TEST (Semaphore, ctor_7)
{
	semaphore sem(7);
	EXPECT_EQ(sem.available(), 7);
	EXPECT_EQ(sem.resources(), 7);
}

// Test that semaphore post throws an overflow exception if it reaches its maximum
TEST (Semaphore, max_overflow)
{
	semaphore sem(7);
	ASSERT_THROW(sem.post(), overflow);
}

// Test that semaphore post doesn't throw in normal conditions
TEST (Semaphore, nooverflow)
{
	semaphore sem(7);
	EXPECT_TRUE(sem.trywait());
	ASSERT_NO_THROW(sem.post());
}

// Test that semaphore wait is postable
TEST (Semaphore, wait_post)
{
	semaphore sem(1);
	class Thread : public strand
	{
	public:
		bool done;
		Thread (semaphore& sem) : done(false), sem(sem) {}
	private:
		semaphore& sem;
		void main ()
		{
			sem.wait(); // wait for a resource to become available and use it
			done = true;
			sem.post(); // free the used resource
		}
	};
	sem.wait(); // consume the only available resource
	EXPECT_EQ(sem.resources(), 1);
	EXPECT_EQ(sem.available(), 0);
	strid handle = strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	sem.post(); // free the used resource, allowing the spawned thread to proceed
	lisle::Exit exit = handle.join();
	EXPECT_EQ(sem.resources(), 1);
	EXPECT_EQ(sem.available(), 1);
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
}

// Test that semaphore trywait is postable
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
	sem.wait();
	strid handle = strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	sem.post();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
	EXPECT_EQ(thread->val, 0);
}

// Test that semaphore wait is cancelable
TEST (Semaphore, wait_cancel)
{
	semaphore sem(1);
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
	sem.wait(); // consume the only available resource
	EXPECT_EQ(sem.resources(), 1);
	EXPECT_EQ(sem.available(), 0);
	strid handle = strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	handle.cancel();
	lisle::Exit exit = handle.join();
	sem.post(); // free the consumed resource
	EXPECT_EQ(sem.resources(), 1);
	EXPECT_EQ(sem.available(), 1);
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
}

// Test that semaphore timed wait times out
TEST (Semaphore, wait_timed)
{
	semaphore sem(1);
	class Thread : public strand
	{
	public:
		bool done;
		bool timedout;
		Thread (semaphore& sem) : done(false), timedout(false), sem(sem) {}
	private:
		semaphore& sem;
		void main ()
		{
			try
			{
				sem.wait(duration(0.2));
				// never reached if thread timed out
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	sem.wait(); // consume the only available resource
	EXPECT_EQ(sem.resources(), 1);
	EXPECT_EQ(sem.available(), 0);
	strid handle = strid(new Thread(sem));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	lisle::Exit exit = handle.join();
	sem.post(); // free the consumed resource
	EXPECT_EQ(sem.resources(), 1);
	EXPECT_EQ(sem.available(), 1);
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_FALSE(thread->done);
	EXPECT_TRUE(thread->timedout);
}
