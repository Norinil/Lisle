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
#include <lisle/Event>
#include <lisle/Strid>
#include <vector>

using namespace lisle;
using namespace std;

#ifdef _MSC_VER
#pragma warning (disable:4512)
#endif

/// Test that event wait is signalable
TEST (EventTest, wait_signal)
{
	Event event;
	class Thread : public Strand
	{
	public:
		bool done;
		Thread (Event& event) : done(false), event(event) {}
	private:
		Event& event;
		void main ()
		{
			event.wait();
			done = true;
		}
	};
	Strid handle = Strid(new Thread(event));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	event.signal();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
}

/// Test that event timedwait is signalable
TEST (EventTest, timedwait_signal)
{
	Event event;
	class Thread : public Strand
	{
	public:
		bool done;
		bool timedout;
		Thread (Event& event) : done(false), timedout(false), event(event) {}
	private:
		Event& event;
		void main ()
		{
			try
			{
				event.wait(Duration(0.02));
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	Strid handle = Strid(new Thread(event));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	sleep(Duration(0.01));
	event.signal();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
	EXPECT_FALSE(thread->timedout);
}

/// Test that event timedwait times out
TEST (EventTest, timedwait_timeout)
{
	Event event;
	class Thread : public Strand
	{
	public:
		bool done;
		bool timedout;
		Thread (Event& event) : done(false), timedout(false), event(event) {}
	private:
		Event& event;
		void main ()
		{
			try
			{
				event.wait(Duration(0.02));
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	Strid handle = Strid(new Thread(event));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	EXPECT_FALSE(thread->timedout);
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_FALSE(thread->done);
	EXPECT_TRUE(thread->timedout);
}

/// Test that broadcasting an event that multiple threads wait on restarts all threads
TEST (EventTest, multiwait_broadcast)
{
	Event event;
	class Thread : public Strand
	{
	public:
		bool done;
		Thread (Event& event) : done(false), event(event) {}
	private:
		Event& event;
		void main ()
		{
			event.wait();
			done = true;
		}
	};
	vector<Strid> pool;
	for (size_t i=0; i<5; ++i)
		pool.push_back(Strid(new Thread(event)));
	for (size_t i=0; i<pool.size(); ++i)
	{
		Thread* thread = dynamic_cast<Thread*>((Strand*)pool[i]);
		EXPECT_FALSE(thread->done);
	}
	event.broadcast();
	for (size_t i=0; i<pool.size(); ++i)
	{
		lisle::Exit exit = pool[i].join();
		EXPECT_EQ(exit, lisle::terminated);
		Thread* thread = dynamic_cast<Thread*>((Strand*)pool[i]);
		EXPECT_TRUE(thread->done);
	}
}

/// Test that event wait is cancelable
TEST (EventTest, wait_cancel)
{
	Event event;
	class Thread : public Strand
	{
	public:
		bool done;
		Thread (Event& event) : done(false), event(event) {}
	private:
		Event& event;
		void main ()
		{
			event.wait();
			// never reached if thread was canceled
			done = true;
		}
	};
	Strid handle = Strid(new Thread(event));
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	sleep(Duration(0.01)); // give the thread a chance to wait
	handle.cancel();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::canceled);
	EXPECT_FALSE(thread->done);
}

/// Test that event timedwait is cancelable
TEST (EventTest, timedwait_cancel)
{
	Event event;
	class Thread : public Strand
	{
	public:
		bool done;
		bool timedout;
		Thread (Event& event) : done(false), timedout(false), event(event) {}
	private:
		Event& event;
		void main ()
		{
			try
			{
				event.wait(Duration(0.02));
				// never reached if thread was canceled
				done = true;
			}
			catch (timeout&)
			{
				timedout = true;
			}
		}
	};
	Strid handle = Strid(new Thread(event));
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
