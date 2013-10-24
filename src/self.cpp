/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2003-2012, Markus Schütz
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
#include <lisle/Thrid>
#include <lisle/Thread>
#include <lisle/Acquirer>
#include <lisle/self>
#include <lisle/thrargs>
#include <new>

using namespace std;

namespace lisle {

void exit (const lisle::Anondle& hretd)
{
	throw lisle::threxit(hretd);
}

void terminate ()
{
	threadle self(lisle::self);
	self->state = thread::exceptioned;
	lisle::exit();
}

void unexpected ()
{
	lisle::terminate();
}

void launch (const lisle::Thread& thread)
{
	// Spawned thread
	// Not static because of a friend declaration
	
	threadle self(lisle::self);
	try
	{
		switch (thread.mode())
		{
			case Thread::Suspended :
			{
				Thrid self;
				self.suspend();
				testcancel();
			} // no break: after being suspended a thread can only enter the running state
			case Thread::Running :
			{
				self->state = thread::running;
				self->hretd = (*thread.call())(thread.main(), thread.args());
			}
			default :
				break;
		}
	}
	catch (lisle::threption&)
	{
		throw; // rethrow cancel or exit for previous stage lisle::startup
	}
	catch (...) // all but cancel or exit are the exceptions we are interested in at this stage
	{
		// Run user's terminate function if supplied.
		// That function may call Thread::exit() or be canceled, which will be handled by the outer try block in lilse::startup.
		// The lisle::terminate() function is called if there is no user supplied function.
		terminate_handler terminator = set_terminate(0); // read terminator a user could have set
		if (terminator)
			terminator(); // if this throws an exception lisle::startup will catch it
		else
			lisle::terminate(); // if the user set a null terminate execute ours anyway to get a clean thread exit
	}
}

void startup (thrargs* args)
{
	// Spawned thread
	
	lisle::Thread thread;

	// Do all initializations here. The thread that created us is waiting
	// for the targs->copied condition to be signaled.
	args->guard.lock();
	lisle::self.create(args->handle);
	threadle self(lisle::self);
	thread = args->start;
	args->copied.signal();
	args->guard.unlock();

	// From here 'args' and 'targs' are no longer usable, and both the thread
	// that created us and ourself are running concurrently.

	switch (thread.state())
	{
		case Thread::Joinable :
			self->detach.state = thread::detach::joinable;
			break;
		case Thread::Detached :
			self->detach.state = thread::detach::detached;
			break;
	}
	set_terminate(lisle::terminate); // invariant for lisle::launch
	set_unexpected(lisle::unexpected);
	try
	{
		launch(thread);
		self->guard.lock();
		self->state = thread::terminated;
	}
	catch (thrcancel&)
	{
		self->guard.lock();
		self->hretd = Anondle();
		self->state = thread::canceled;
	}
	catch (threxit& e)
	{
		self->guard.lock();
		self->hretd = e.hretd;
		if (self->state != thread::exceptioned) // lisle::terminate can exit the thread but it first sets the thread's state to exceptioned
			self->state = thread::terminated;
	}
	catch (...)
	{
		// A system unexpected exception has occured running the user's
		// terminate routine. We get control back within this block.
		// Cleanup and tell this thread is in exceptioned state.
		self->guard.lock();
		self->state = thread::exceptioned;
	}
	if (self->detach.state == thread::detach::joinable)
		self->waitjoiner();
	self->guard.unlock();
	lisle::self.destroy();
}

void exit ()
{
	// Calling thread
	exit(Anondle());
}

void set (const cancel::State& cancelability)
{
	// Calling thread
	threadle self(lisle::self);
	Acquirer thread(self->guard);
	switch (cancelability)
	{
		case cancel::disable :
			self->cancel.state = thread::cancel::disable;
			break;
		case cancel::enable :
			self->cancel.state = thread::cancel::enable;
			break;
	}
}

void get (cancel::State& cancelability)
{
	// Calling thread
	threadle self(lisle::self);
	Acquirer thread(self->guard);
	switch (self->cancel.state)
	{
		case thread::cancel::disable :
			cancelability = cancel::disable;
			break;
		case thread::cancel::enable :
			cancelability = cancel::enable;
			break;
	}
}

void testcancel ()
{
	// Calling thread
	threadle self(lisle::self);
	self->testcancel();
}

void thread::waitrestartcancel (thread* waiting, lisle::Mutex& device)
{
	// Restartable-cancelable wait
	
	this->testcancel();
	{
		Acquirer thread(this->guard);
		if (this->cancel.state == thread::cancel::enable)
		{
			this->cancel.waiting.queue = waiting;
			this->cancel.waiting.guard = &device;
		}
		this->waitrestart();   // Expects locked guard
		this->cancel.waiting.guard = NULL;
		this->cancel.waiting.queue = NULL;
	}
	this->testcancel();
}

void thread::waitrestartcancel (const Duration& duration, thread* waiting, lisle::Mutex& device)
{
	// Restartable-cancellable timed wait
	
	this->testcancel();
	{
		Acquirer thread(this->guard);
		if (duration <= Duration(0))
			throw lisle::timeout();
		if (this->cancel.state == thread::cancel::enable)
		{
			this->cancel.waiting.queue = waiting;
			this->cancel.waiting.guard = &device;
		}
		this->waitrestart(duration);   // Expects locked guard
		this->cancel.waiting.guard = NULL;
		this->cancel.waiting.queue = NULL;
	}
	this->testcancel();
}

void thread::testcancel ()
{
	Acquirer thread(this->guard);
	if (this->cancel.pending)
	{
		this->state = thread::canceling;
		this->cancel.state = thread::cancel::disable;
		this->cancel.pending = false;
		throw lisle::thrcancel();
	}
}

void thread::waitjoiner ()
{
	// Wait for a joiner to join us
	// Warning: this->guard *must* have been locked before calling this function.
	//          Works only if this == self thread.
	
	if ((this->joiner != NULL) && !this->joiner->cancel.pending)
		this->joiner->restart();
	else
		this->waitrestart();
}

}
