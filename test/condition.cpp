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
#include <lisle/condition.h>
#include <lisle/acquirer.h>
#include <lisle/strid.h>
#include <vector>

using namespace lisle;

#ifdef _MSC_VER
#pragma warning (disable:4512)
#endif

/// Test that waiting throws a permission exception if mutex was not locked
TEST (Condition, wait_freemutex)
{
	mutex guard;
	condition cond(guard);
	ASSERT_THROW(cond.wait(), permission);
	ASSERT_THROW(cond.wait(duration(0.1)), permission);
}

/// Test that condition wait is signalable
TEST (Condition, wait_signal)
{
	mutex guard;
	condition cond(guard);
	class Thread : public strand
	{
	public:
		bool done;
		Thread (mutex& guard, condition& cond) : done(false), guard(guard), cond(cond) {}
	private:
		mutex& guard;
		condition& cond;
		void main ()
		{
			acquirer lock(guard);
			cond.wait();
			done = true;
		}
	};
	strid handle = strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	guard.acquire();
	cond.signal();
	guard.release();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
}

/// Test that condition timedwait is signalable
TEST (Condition, timedwait_signal)
{
	mutex guard;
	condition cond(guard);
	class Thread : public strand
	{
	public:
		bool done;
		bool timedout;
		Thread (mutex& guard, condition& cond) : done(false), timedout(false), guard(guard), cond(cond) {}
	private:
		mutex& guard;
		condition& cond;
		void main ()
		{
			try
			{
				acquirer lock(guard);
				cond.wait(duration(0.5));
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	strid handle = strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	sleep(0.1);
	guard.acquire();
	cond.signal();
	guard.release();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
	EXPECT_FALSE(thread->timedout);
}

/// Test that condition timedwait times out
TEST (Condition, timedwait_timeout)
{
	mutex guard;
	condition cond(guard);
	class Thread : public strand
	{
	public:
		bool done;
		bool timedout;
		Thread (mutex& guard, condition& cond) : done(false), timedout(false), guard(guard), cond(cond) {}
	private:
		mutex& guard;
		condition& cond;
		void main ()
		{
			try
			{
				acquirer lock(guard);
				cond.wait(duration(0.02));
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	strid handle = strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_FALSE(thread->done);
	EXPECT_TRUE(thread->timedout);
}

/// Test that broadcasting a condition that multiple threads wait on restarts all threads
TEST (Condition, multiwait_broadcast)
{
	const size_t maxthreads = 5; // how many threads will start
	mutex guard;
	condition cond(guard);
	struct waits {
		size_t count = 0;
		mutex guard;
		condition ready;
		waits () : ready(guard) {}
		void operator ++ (int)
		{
			guard.acquire();
			count++;
			guard.release();
		}
		void operator -- (int)
		{
			guard.acquire();
			count--;
			guard.release();
		}
	} waiters;

	class Thread : public strand
	{
	public:
		bool done;
		Thread (mutex& guard, condition& cond, waits& waiters) : done(false), guard(guard), cond(cond), waiters(waiters) {}
	private:
		mutex& guard;
		condition& cond;
		waits& waiters;
		void main ()
		{
			acquirer lock(guard);
			waiters++;
			if (waiters.count == maxthreads)
			{
				acquirer lock(waiters.guard);
				waiters.ready.notify();
			}
			cond.wait();
			waiters--;
			done = true;
			if (waiters.count == 0)
			{
				acquirer lock(waiters.guard);
				waiters.ready.notify();
			}
		}
	};

	std::vector<strid> pool;
	for (size_t i=0; i<maxthreads; ++i)
		pool.push_back(strid(new Thread(guard, cond, waiters)));

	// here waiters.ready gets signaled when waiters.count == maxthreads
	waiters.guard.acquire();
	if (waiters.count < maxthreads)
		waiters.ready.wait();
	waiters.guard.release();

	for (size_t i=0; i<pool.size(); ++i)
	{
		Thread* thread = dynamic_cast<Thread*>((strand*)pool[i]);
		EXPECT_FALSE(thread->done);
	}
	guard.acquire();
	cond.broadcast();
	guard.release();

	// here waiters.ready gets signaled when waiters.count == 0
	waiters.guard.acquire();
	if (waiters.count > 0)
		waiters.ready.wait();
	waiters.guard.release();

	for (size_t i=0; i<pool.size(); ++i)
	{
		lisle::Exit exit = pool[i].join();
		EXPECT_EQ(exit, lisle::terminated);
		Thread* thread = dynamic_cast<Thread*>((strand*)pool[i]);
		EXPECT_TRUE(thread->done);
	}
}

/// Test that condition wait is cancelable
TEST (Condition, wait_cancel)
{
	mutex guard;
	condition cond(guard);
	class Thread : public strand
	{
	public:
		bool done;
		Thread (mutex& guard, condition& cond) : done(false), guard(guard), cond(cond) {}
	private:
		mutex& guard;
		condition& cond;
		void main ()
		{
			acquirer lock(guard);
			cond.wait();
			// never reached if thread was canceled
			done = true;
		}
	};
	strid handle = strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	sleep(duration(0.1)); // give the thread a chance to wait
	handle.cancel();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
}

/// Test that condition timedwait is cancelable
TEST (Condition, timedwait_cancel)
{
	mutex guard;
	condition cond(guard);
	class Thread : public strand
	{
	public:
		bool done;
		bool timedout;
		Thread (mutex& guard, condition& cond) : done(false), timedout(false), guard(guard), cond(cond) {}
	private:
		mutex& guard;
		condition& cond;
		void main ()
		{
			try
			{
				acquirer lock(guard);
				cond.wait(duration(0.5));
				// never reached if thread was canceled
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	strid handle = strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	sleep(duration(0.1)); // give the thread a chance to wait
	handle.cancel();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
}
