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
#include <lisle/countic.h>
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
	cond.signal();
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
				cond.wait(duration(0.2));
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
	sleep(duration(0.1));
	cond.signal();
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
	countic waiters; // count waiting threads
	class Thread : public strand
	{
	public:
		bool done;
		Thread (mutex& guard, condition& cond, countic& waiters) : done(false), guard(guard), cond(cond), waiters(waiters) {}
	private:
		mutex& guard;
		condition& cond;
		countic& waiters;
		void main ()
		{
			acquirer lock(guard);
			waiters.inc();
			cond.wait();
			waiters.dec();
			done = true;
		}
	};
	std::vector<strid> pool;
	for (size_t i=0; i<maxthreads; ++i)
	{
		pool.push_back(strid(new Thread(guard, cond, waiters)));
		yield();
	}
	while (waiters < maxthreads)
		yield();
	for (size_t i=0; i<pool.size(); ++i)
	{
		Thread* thread = dynamic_cast<Thread*>((strand*)pool[i]);
		EXPECT_FALSE(thread->done);
	}
	cond.broadcast();
	while (waiters > 0)
		yield();
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
	sleep(duration(0.01)); // give the thread a chance to wait
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
				cond.wait(duration(0.02));
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
	sleep(duration(0.01)); // give the thread a chance to wait
	handle.cancel();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
}
