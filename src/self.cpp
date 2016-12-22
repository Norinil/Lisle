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
#include <lisle/thrid.h>
#include <lisle/thread.h>
#include <lisle/acquirer.h>
#include <lisle/releaser.h>
#include <lisle/self.h>
#include <new>
#include "self.h"
#include "thrargs.h"

#ifdef _MSC_VER
#pragma warning (disable : 4459)
#endif

using namespace std;

namespace lisle {

void suspend ()
{
	// Synchronous suspend by calling thread
	intern::threadle thr(intern::self);
	acquirer lock(thr->guard);
	if ((thr->state < intern::thread::canceling) && (thr->state != intern::thread::suspended))
	{
		thr->state = intern::thread::suspended;
		thr->suspend();
		thr->state = intern::thread::running;
	}
}

void exit (const lisle::anondle& hretd)
{
	throw lisle::threxit(hretd);
}

void terminate ()
{
	intern::threadle self(intern::self);
	self->state = intern::thread::exceptioned;
	lisle::exit();
}

void unexpected ()
{
	lisle::terminate();
}

void launch (const lisle::thread& thread)
{
	// Spawned thread
	// Not static because of a friend declaration
	
	intern::threadle self(intern::self);
	try
	{
		switch (thread.mode())
		{
			case thread::suspended :
			{
				suspend();
				testcancel();
			} // no break: after being suspended a thread can only enter the running state
			case thread::running :
			{
				self->state = intern::thread::running;
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
	
	lisle::thread thread;

	// Do all initializations here. The thread that created us is waiting
	// for the targs->copied condition to be signaled.
	args->guard.lock();
	intern::self.create(args->handle);
	intern::threadle self(intern::self);
	thread = args->start;
	args->copied.signal();
	args->guard.unlock();

	// From here 'args' and 'targs' are no longer usable, and both the thread
	// that created us and ourself are running concurrently.

	switch (thread.state())
	{
		case thread::joinable :
			self->detach.state = intern::thread::detach::joinable;
			break;
		case thread::detached :
			self->detach.state = intern::thread::detach::detached;
			break;
	}
	set_terminate(lisle::terminate); // invariant for lisle::launch
	set_unexpected(lisle::unexpected);
	try
	{
		launch(thread);
		self->guard.lock();
		self->state = intern::thread::terminated;
	}
	catch (thrcancel&)
	{
		self->guard.lock();
		self->hretd = anondle();
		self->state = intern::thread::canceled;
	}
	catch (threxit& e)
	{
		self->guard.lock();
		self->hretd = e.hretd;
		if (self->state != intern::thread::exceptioned) // lisle::terminate can exit the thread but it first sets the thread's state to exceptioned
			self->state = intern::thread::terminated;
	}
	catch (...)
	{
		// A system unexpected exception has occured running the user's
		// terminate routine. We get control back within this block.
		// Cleanup and tell this thread is in exceptioned state.
		self->guard.lock();
		self->state = intern::thread::exceptioned;
	}
	if (self->detach.state == intern::thread::detach::joinable)
		self->waitjoiner();
	self->guard.unlock();
	intern::self.destroy();
}

void exit ()
{
	// Calling thread
	exit(anondle());
}

void set (const cancel::State& cancelability)
{
	// Calling thread
	intern::threadle self(intern::self);
	acquirer thread(self->guard);
	switch (cancelability)
	{
		case cancel::disable :
			self->cancel.state = intern::thread::cancel::disable;
			break;
		case cancel::enable :
			self->cancel.state = intern::thread::cancel::enable;
			break;
	}
}

void get (cancel::State& cancelability)
{
	// Calling thread
	intern::threadle self(intern::self);
	acquirer thread(self->guard);
	switch (self->cancel.state)
	{
		case intern::thread::cancel::disable :
			cancelability = cancel::disable;
			break;
		case intern::thread::cancel::enable :
			cancelability = cancel::enable;
			break;
	}
}

void testcancel ()
{
	// Calling thread
	intern::threadle self(intern::self);
	self->testcancel();
}

void intern::thread::waitrestartcancel (intern::thread* waiting, mutex& device)
{
	// Restartable-cancelable wait
	
	this->testcancel();
	{
		acquirer thread(this->guard);
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

void intern::thread::waitrestartcancel (const duration& span, intern::thread* waiting, mutex& device)
{
	// Restartable-cancellable timed wait
	
	this->testcancel();
	{
		acquirer thread(this->guard);
		if (span <= duration(0))
			throw lisle::timeout();
		if (this->cancel.state == thread::cancel::enable)
		{
			this->cancel.waiting.queue = waiting;
			this->cancel.waiting.guard = &device;
		}
		this->waitrestart(span);   // Expects locked guard
		this->cancel.waiting.guard = NULL;
		this->cancel.waiting.queue = NULL;
	}
	this->testcancel();
}

void intern::thread::testcancel ()
{
	acquirer thread(this->guard);
	if (this->cancel.pending)
	{
		this->state = thread::canceling;
		this->cancel.state = thread::cancel::disable;
		this->cancel.pending = false;
		throw lisle::thrcancel();
	}
}

void intern::thread::waitjoiner ()
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
