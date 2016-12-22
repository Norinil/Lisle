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
#include <lisle/self.h>
#include <lisle/acquirer.h>
#include <lisle/releaser.h>
#include "../../src/self.h"
#include <ctime>
#include <cerrno>
#include <csignal>
#include <sched.h>

#define MAX_TVSEC  0x7fffffff
#define MAX_TVNSEC 999999999

namespace lisle {
namespace intern {

// The global thread self variable
Self self;

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

void thread::suspend ()
{
	// Warning: this->guard *must* have been locked before calling this function.
	// Disable signals to avoid suprious wakeups.
	struct set {
		sigset_t news;
		sigset_t olds;
	} set;
	sigfillset(&set.news);
	sigdelset(&set.news, SIGINT); // allow Ctrl-C
	pthread_sigmask(SIG_SETMASK, &set.news, &set.olds);
	pthread_cond_wait(&base::restart.condition, &this->guard.data);
	pthread_sigmask(SIG_SETMASK, &set.olds, 0);
}

void thread::resume ()
{
	pthread_cond_signal(&base::restart.condition);
}

void thread::restart ()
{
	pthread_cond_signal(&base::restart.condition);
}

void thread::waitrestart ()
{
	// Wait for restart signal
	// Warning: this->guard *must* have been locked before calling this function.
	// Disable signals to avoid suprious wakeups.
	struct set {
		sigset_t news;
		sigset_t olds;
	} set;
	sigfillset(&set.news);
	sigdelset(&set.news, SIGINT); // allow Ctrl-C
	pthread_sigmask(SIG_SETMASK, &set.news, &set.olds);
	pthread_cond_wait(&base::restart.condition, &this->guard.data);
	pthread_sigmask(SIG_SETMASK, &set.olds, 0);
}

void thread::waitrestart (const duration& span)
{
	// Wait for restart signal or timeout
	// Warning: this->guard *must* have been locked before calling this function.
	// Disable signals to avoid suprious wakeups.
	timespec abstime;
	clock_gettime(CLOCK_REALTIME, &abstime);
	abstime.tv_nsec += span.nsec();
	if (abstime.tv_nsec > MAX_TVNSEC)
	{
		abstime.tv_nsec %= (MAX_TVNSEC+1);
		++abstime.tv_sec;
	}
	abstime.tv_sec += span.sec();
	struct set {
		sigset_t news;
		sigset_t olds;
	} set;
	sigfillset(&set.news);
	sigdelset(&set.news, SIGINT); // allow Ctrl-C
	pthread_sigmask(SIG_SETMASK, &set.news, &set.olds);
	int rc = pthread_cond_timedwait(&base::restart.condition, &this->guard.data, &abstime);
	pthread_sigmask(SIG_SETMASK, &set.olds, 0);
	if (rc == ETIMEDOUT)
		throw timeout();
}

void thread::waitjoiningcancel (thread* thread)
{
	// Wait for joining thread or cancel
	// Warning: thread->guard *must* have been locked before calling this function.
	//          Works only if this == self thread.
	
	this->testcancel();
	if (thread->state < thread::terminal)
	{
		acquirer self(this->guard);
		thread->joiner = this;
		if (this->cancel.state == thread::cancel::enable)
			this->cancel.joining = thread;
		{
			releaser release(thread->guard);
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
}

namespace lisle {

void yield ()
{
	sched_yield();
}

void sleep (const duration& span)
{
	timespec req;
	timespec rem;
	req.tv_sec = span.sec();
	req.tv_nsec = span.nsec();
	int rc = nanosleep(&req, &rem);
}

}
