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
#include <lisle/self>
#include <signals.h>
#include <lisle/Acquirer>
#include <lisle/Releaser>
#include <csignal>
#include <sched.h>

#define MAX_TVSEC  0x7fffffff
#define MAX_TVNSEC 999999999

namespace lisle {

// The global thread self variable
class Self self;

Self::~Self ()
{
	// Executed in main thread only. Static variables destruction.
	destroy();
	pthread_key_delete(key);
}

Self::Self ()
{
	// Executed in main thread only. Static variables initialization.
	threadle self(new thread);
	pthread_key_create(&key, 0);
	self->state = thread::running;
	create(self);
}

Self::operator threadle () const
{
	/* Wierd case:
	 * If global synchronization devices are used, their destructor can be called
	 * after the global _lisle_::self destructor, in which case this operator
	 * returns 0, what generates a memory access violation. To avoid that we
	 * generate a new thread handle on the fly, which at least stores a valid
	 * thread id. In synchronization device destructors we just want know the id
	 * of the faulty thread.
	 */
	threadle* stored;
	stored = (threadle*)pthread_getspecific(key);
	if (stored != 0)
		return *stored;
	else
		return threadle(new thread);
}

void Self::create (const threadle& handle)
{
	threadle* store;
	store = new threadle(handle);
	pthread_setspecific(key, store);
}

void Self::destroy ()
{
	threadle* stored;
	stored = (threadle*)pthread_getspecific(key);
	delete stored;
}

void yield ()
{
	sched_yield();
}

void sleep (const Duration& duration)
{
	timespec req;
	timespec rem;
	req.tv_sec = duration.sec();
	req.tv_nsec = duration.nsec();
	int rc = nanosleep(&req, &rem);
}

void thread::yield ()
{
	sched_yield();
}

void thread::suspend ()
{
	// Asynchronous suspend
	pthread_kill(this->handle, LISLE_SIG_SUSPEND);
}

void thread::resume ()
{
	// Resume asynchronous suspend
	pthread_kill(this->handle, LISLE_SIG_RESTART);
	while (this->state != thread::running)
		sched_yield();
}

void thread::restart ()
{
	pthread_kill(this->handle, LISLE_SIG_RESTART);
}

void thread::waitrestart ()
{
	// Wait for restart signal
	// Warning: this->guard *must* have been locked before calling this function.
	
	sigset_t mask;
	int signal;

	pthread_sigmask(SIG_SETMASK, NULL, &mask);
	sigaddset(&mask, LISLE_SIG_RESTART);
	{
		Releaser release(this->guard);
		sigwait(&mask, &signal);
	}
}

void thread::waitrestart (const Duration& duration)
{
	// Wait for restart signal or timeout
	// Warning: this->guard *must* have been locked before calling this function.
	
	timespec rem;
	timespec req;
	int signal = 0;
	rem.tv_sec = duration.sec();
	rem.tv_nsec = duration.nsec();
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, LISLE_SIG_RESTART);
	sigaddset(&mask, 36);
	do
	{
		bool signaled;
		req = rem;
		{
			Releaser release(this->guard);
			pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
			signaled = nanosleep(&req, &rem) != 0;
			pthread_sigmask(SIG_BLOCK, &mask, NULL);
		}
		signal = this->signal; // this->signal is set by self->signal assignment in posix/signal.cpp function signaled
		this->signal = 0;
		if (signaled)
		{ // we're a cancelation point: test for cancelation
			Releaser release(this->guard);
			testcancel();
		}
		else // not signaled means timed-out
		{
			throw lisle::timeout();
		}
	}
	while (signal != LISLE_SIG_RESTART);
}

void thread::waitjoiningcancel (thread* thread)
{
	// Wait for joining thread or cancel
	// Warning: thread->guard *must* have been locked before calling this function.
	//          Works only if this == self thread.
	
	this->testcancel();
	if (thread->state < thread::terminal)
	{
		Acquirer self(this->guard);
		thread->joiner = this;
		if (this->cancel.state == thread::cancel::enable)
			this->cancel.joining = thread;
		{
			Releaser release(thread->guard);
			this->waitrestart();
		}
		this->cancel.joining = NULL;
		thread->joiner = NULL;
	}
	else   // thread is waiting on us to restart it to perform join
		thread->restart();
	this->testcancel();
}

}
