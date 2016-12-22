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
#include <lisle/self>
#include <lisle/create>
#include <lisle/Strid>

#ifdef _MSC_VER
#pragma warning (disable:4211)
#pragma warning (disable:4505)
#endif

using namespace lisle;

/// Ensure yield doesn't throw an exception.
TEST (selfTest, yield)
{
	EXPECT_NO_THROW(yield());
}

/// Ensure uncaught exceptions don't crash a thread but exit it properly.
/// Thread::Exceptioned is supposed to be returned by Thrid::join() in the case
/// of uncaught exceptions in a thread... as long as the thread is joinable.
static void throwerDefault ()
{
	throw false;
}
TEST (selfTest, terminateDefault)
{
	Thrid tid = create(Thread(throwerDefault));
	Exit exit = tid.join();
	EXPECT_EQ(exit, exceptioned);
}

/// Ensure uncaught exceptions don't crash a thread but exit it properly
/// even when a user sets the terminate handler to null.
static void throwerTerminateNull ()
{
	std::set_terminate(0);
	throw false;
}
TEST (selfTest, terminateTerminateNull)
{
	Thrid tid = create(Thread(throwerDefault));
	Exit exit = tid.join();
	EXPECT_EQ(exit, exceptioned);
}

/// Ensure uncaught exceptions can be handled in a terminate handler.
static int count_terminate = 0;
static std::terminate_handler handle_terminate = 0;
void terminate ()
{
	count_terminate++;
	if (handle_terminate)
		handle_terminate();
}
static void throwerTerminate ()
{
	handle_terminate = std::set_terminate(terminate);
	throw false;
}
TEST (selfTest, terminateTerminate)
{
	count_terminate = 0;
	handle_terminate = 0;
	Thrid tid = create(Thread(throwerTerminate));
	Exit exit = tid.join();
	EXPECT_EQ(exit, exceptioned);
	EXPECT_EQ(count_terminate, 1);
}

TEST(selfTest, enableCancel)
{
	set(cancel::enable);
	cancel::State cancelability;
	get(cancelability);
	EXPECT_EQ(cancelability, cancel::enable);
}

TEST(selfTest, disableCancel)
{
	set(cancel::disable);
	cancel::State cancelability;
	get(cancelability);
	EXPECT_EQ(cancelability, cancel::disable);
}

/// Test the synchronous suspend asynchronous resume pair
TEST (selfTest, suspend_resume)
{
	class Thread : public Strand
	{
	public:
		bool done;
		Thread () : done(false) {}
	private:
		void main ()
		{
			suspend();
			done = true;
		}
	};
	Strid handle = Strid(new Thread);
	Thread* thread = dynamic_cast<Thread*>((Strand*)handle);
	EXPECT_FALSE(thread->done);
	handle.resume();
	lisle::Exit exit = handle.join();
	EXPECT_EQ(exit, lisle::terminated);
	EXPECT_TRUE(thread->done);
}
