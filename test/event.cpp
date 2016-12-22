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
#include <lisle/event.h>
#include <lisle/strid.h>
#include <vector>

using namespace lisle;
using namespace std;

#ifdef _MSC_VER
#pragma warning (disable:4512)
#endif

/// Test that event wait is signalable
TEST (Event, wait_signal)
{
	event act;
	class Thread : public strand
	{
	public:
		bool done;
		Thread (event& act) : done(false), act(act) {}
	private:
		event& act;
		void main ()
		{
			act.wait();
			done = true;
		}
	};
	strid handle = strid(new Thread(act));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	act.signal();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
}

/// Test that event timedwait is signalable
TEST (Event, timedwait_signal)
{
	event act;
	class Thread : public strand
	{
	public:
		bool done;
		bool timedout;
		Thread (event& act) : done(false), timedout(false), act(act) {}
	private:
		event& act;
		void main ()
		{
			try
			{
				act.wait(duration(0.02));
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	strid handle = strid(new Thread(act));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	sleep(duration(0.01));
	act.signal();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
	EXPECT_FALSE(thread->timedout);
}

/// Test that event timedwait times out
TEST (Event, timedwait_timeout)
{
	event act;
	class Thread : public strand
	{
	public:
		bool done;
		bool timedout;
		Thread (event& act) : done(false), timedout(false), act(act) {}
	private:
		event& act;
		void main ()
		{
			try
			{
				act.wait(duration(0.02));
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	strid handle = strid(new Thread(act));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_FALSE(thread->done);
	EXPECT_TRUE(thread->timedout);
}

/// Test that broadcasting an event that multiple threads wait on restarts all threads
TEST (Event, multiwait_broadcast)
{
	event act;
	class Thread : public strand
	{
	public:
		bool done;
		Thread (event& act) : done(false), act(act) {}
	private:
		event& act;
		void main ()
		{
			act.wait();
			done = true;
		}
	};
	vector<strid> pool;
	for (size_t i=0; i<5; ++i)
		pool.push_back(strid(new Thread(act)));
	for (size_t i=0; i<pool.size(); ++i)
	{
		Thread* thread = dynamic_cast<Thread*>((strand*)pool[i]);
		EXPECT_FALSE(thread->done);
		sleep(duration(0.01)); // give the spawned thread a chance to wait on the event
	}
	act.broadcast();
	for (size_t i=0; i<pool.size(); ++i)
	{
		lisle::Exit exit = pool[i].join();
		EXPECT_EQ(exit, lisle::terminated);
		Thread* thread = dynamic_cast<Thread*>((strand*)pool[i]);
		EXPECT_TRUE(thread->done);
	}
}

/// Test that event wait is cancelable
TEST (Event, wait_cancel)
{
	event act;
	class Thread : public strand
	{
	public:
		bool done;
		Thread (event& act) : done(false), act(act) {}
	private:
		event& act;
		void main ()
		{
			act.wait();
			// never reached if thread was canceled
			done = true;
		}
	};
	strid handle = strid(new Thread(act));
	Thread* thread = dynamic_cast<Thread*>((strand*)handle);
	EXPECT_FALSE(thread->done);
	sleep(duration(0.01)); // give the thread a chance to wait
	handle.cancel();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
}

/// Test that event timedwait is cancelable
TEST (Event, timedwait_cancel)
{
	event act;
	class Thread : public strand
	{
	public:
		bool done;
		bool timedout;
		Thread (event& act) : done(false), timedout(false), act(act) {}
	private:
		event& act;
		void main ()
		{
			try
			{
				act.wait(duration(0.02));
				// never reached if thread was canceled
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	strid handle = strid(new Thread(act));
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
