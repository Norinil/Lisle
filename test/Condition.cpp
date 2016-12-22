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
#include <lisle/Condition>
#include <lisle/Acquirer>
#include <lisle/Strid>
#include <lisle/Countic>
#include <vector>

using namespace lisle;
using namespace std;

#ifdef _MSC_VER
#pragma warning (disable:4512)
#endif

/// Test that waiting throws a permission exception if mutex was not locked
TEST (ConditionTest, wait_freemutex)
{
	Mutex mutex;
	Condition cond(mutex);
	ASSERT_THROW(cond.wait(), permission);
	ASSERT_THROW(cond.wait(Duration(0.1)), permission);
}

/// Test that condition wait is signalable
TEST (ConditionTest, wait_signal)
{
	Mutex guard;
	Condition cond(guard);
	class Thread : public Strand
	{
	public:
		bool done;
		Thread (Mutex& guard, Condition& cond) : done(false), guard(guard), cond(cond) {}
	private:
		Mutex& guard;
		Condition& cond;
		void main ()
		{
			Acquirer lock(guard);
			cond.wait();
			done = true;
		}
	};
	Strid handle = Strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	cond.signal();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
}

/// Test that condition timedwait is signalable
TEST (ConditionTest, timedwait_signal)
{
	Mutex guard;
	Condition cond(guard);
	class Thread : public Strand
	{
	public:
		bool done;
		bool timedout;
		Thread (Mutex& guard, Condition& cond) : done(false), timedout(false), guard(guard), cond(cond) {}
	private:
		Mutex& guard;
		Condition& cond;
		void main ()
		{
			try
			{
				Acquirer lock(guard);
				cond.wait(Duration(0.2));
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	Strid handle = Strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	sleep(Duration(0.1));
	cond.signal();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
	EXPECT_FALSE(thread->timedout);
}

/// Test that condition timedwait times out
TEST (ConditionTest, timedwait_timeout)
{
	Mutex guard;
	Condition cond(guard);
	class Thread : public Strand
	{
	public:
		bool done;
		bool timedout;
		Thread (Mutex& guard, Condition& cond) : done(false), timedout(false), guard(guard), cond(cond) {}
	private:
		Mutex& guard;
		Condition& cond;
		void main ()
		{
			try
			{
				Acquirer lock(guard);
				cond.wait(Duration(0.02));
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	Strid handle = Strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_FALSE(thread->done);
	EXPECT_TRUE(thread->timedout);
}

/// Test that broadcasting a condition that multiple threads wait on restarts all threads
TEST (ConditionTest, multiwait_broadcast)
{
	const size_t maxthreads = 5; // how many threads will start
	Mutex guard;
	Condition cond(guard);
	Countic waiters; // count waiting threads
	class Thread : public Strand
	{
	public:
		bool done;
		Thread (Mutex& guard, Condition& cond, Countic& waiters) : done(false), guard(guard), cond(cond), waiters(waiters) {}
	private:
		Mutex& guard;
		Condition& cond;
		Countic& waiters;
		void main ()
		{
			Acquirer lock(guard);
			waiters.inc();
			cond.wait();
			waiters.dec();
			done = true;
		}
	};
	vector<Strid> pool;
	for (size_t i=0; i<maxthreads; ++i)
	{
		pool.push_back(Strid(new Thread(guard, cond, waiters)));
		yield();
	}
	while (waiters < maxthreads)
		yield();
	for (size_t i=0; i<pool.size(); ++i)
	{
		Thread* thread = dynamic_cast<Thread*>((Strand*)pool[i]);
		EXPECT_FALSE(thread->done);
	}
	cond.broadcast();
	while (waiters > 0)
		yield();
	for (size_t i=0; i<pool.size(); ++i)
	{
		lisle::Exit exit = pool[i].join();
		EXPECT_EQ(exit, lisle::terminated);
		Thread* thread = dynamic_cast<Thread*>((Strand*)pool[i]);
		EXPECT_TRUE(thread->done);
	}
}

/// Test that condition wait is cancelable
TEST (ConditionTest, wait_cancel)
{
	Mutex guard;
	Condition cond(guard);
	class Thread : public Strand
	{
	public:
		bool done;
		Thread (Mutex& guard, Condition& cond) : done(false), guard(guard), cond(cond) {}
	private:
		Mutex& guard;
		Condition& cond;
		void main ()
		{
			Acquirer lock(guard);
			cond.wait();
			// never reached if thread was canceled
			done = true;
		}
	};
	Strid handle = Strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	sleep(Duration(0.01)); // give the thread a chance to wait
	handle.cancel();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
}

/// Test that condition timedwait is cancelable
TEST (ConditionTest, timedwait_cancel)
{
	Mutex guard;
	Condition cond(guard);
	class Thread : public Strand
	{
	public:
		bool done;
		bool timedout;
		Thread (Mutex& guard, Condition& cond) : done(false), timedout(false), guard(guard), cond(cond) {}
	private:
		Mutex& guard;
		Condition& cond;
		void main ()
		{
			try
			{
				Acquirer lock(guard);
				cond.wait(Duration(0.02));
				// never reached if thread was canceled
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	Strid handle = Strid(new Thread(guard, cond));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	sleep(Duration(0.01)); // give the thread a chance to wait
	handle.cancel();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
}
