/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2012, Markus Schütz
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
#include <lisle/Acquirer>
#include <lisle/Releaser>
#include <cstdio>

#ifdef _MSC_VER
#pragma warning (disable : 4459)
#endif

namespace lisle {

// The global thread self variable
class Self self;

Self::~Self ()
{
	// Executed in main thread only. Static variables destruction.
	destroy();
	TlsFree(key);
}

// Executed in main thread only. Static variables initialization.
Self::Self ()
{
	using namespace std;
	threadle self(new thread);
	key = TlsAlloc();
	self->state = thread::running;
	create(self);
}

Self::operator threadle () const
{
	/* Wierd case:
	 * If global synchronization devices are used, their destructor can be called
	 * after the global lisle::self destructor, in which case this operator
	 * returns NULL, what generates a memory access violation. To avoid that we
	 * generate a new thread handle on the fly, which at least stores a valid
	 * thread id. In synchronization device destructors we just want know the id
	 * of the faulty thread.
	 */
	using namespace std;
	threadle* stored;
	stored = (threadle*)TlsGetValue(key);
	if (stored != NULL)
		return *stored;
	else
		return threadle(new thread);
}

void Self::create (const threadle& handle)
{
	using namespace std;
	threadle* store;
	store = new threadle(handle);
	TlsSetValue(key, store);
}

void Self::destroy ()
{
	using namespace std;
	threadle* stored;
	stored = (threadle*)TlsGetValue(key);
	delete stored;
}

void yield ()
{
	// Calling thread
	Sleep(0);
}

void sleep (const Duration& duration)
{
	// Let the calling thread sleep for the given duration.
	DWORD msecs;
	uint64_t slices, i;
	
	// 0x0020'0000 * 1000 < 0xffff'ffff
	msecs = ((duration.sec() % 0x00200000) * 1000) + ((duration.nsec() + 500000) / 1000000);
	slices = (duration.sec() / 0x00200000) + 1;
	for (i=0; i<slices; ++i)
	{
		Sleep(msecs);
		msecs = 0x00200000 * 1000;
	}
}

void thread::yield ()
{
	Sleep(0);
}

void thread::suspend ()
{
	// Warning: this->guard *must* have been locked before calling this function.
	Releaser release(this->guard);
	SuspendThread(this->handle);
}

void thread::resume ()
{
	// Resume suspend
	ResumeThread(this->handle);
}

void thread::restart ()
{
	SetEvent(base::restart.event);
}

void thread::waitrestart ()
{
	// Wait for restart event
	// Warning: this->guard *must* have been locked before calling this function.

	DWORD status;
	{
		lisle::Releaser release(this->guard);
		status = WaitForSingleObject(base::restart.event, INFINITE);
	}
	switch (status)
	{
		case WAIT_FAILED :
		case WAIT_TIMEOUT :
		case WAIT_ABANDONED :
		default :
			// Should not happen
			printf("Lisle internal error: %s, line %u\n", __FILE__, __LINE__);
			throw errno;
		break;
		case WAIT_OBJECT_0 :
			// Got the restart event
		break;
	}
}

void thread::waitrestart (const Duration& duration)
{
	// Wait for restart event or timeout
	// Warning: this->guard *must* have been locked before calling this function.

	DWORD status;
	DWORD msecs;
	uint64_t slices, i;
	bool signaled;

	// 0x0020'0000 * 1000 < 0xffff'ffff
	msecs = ((duration.sec() % 0x00200000) * 1000) + ((duration.nsec() + 500000) / 1000000);
	slices = (duration.sec() / 0x00200000) + 1;
	signaled = false;
	for (i=0; i<slices && !signaled; i++)
	{
		{
			lisle::Releaser release(this->guard);
			status = WaitForSingleObject(base::restart.event, msecs);
		}
		switch (status)
		{
			case WAIT_FAILED :
			case WAIT_ABANDONED :
			default :
				// Should not happen
				printf("Lisle internal error: %s, line %u\n", __FILE__, __LINE__);
				throw errno;
			break;
			case WAIT_TIMEOUT :
				if (i == slices-1)
					throw lisle::timeout();
				else
					msecs = 0x00200000 * 1000;
			break;
			case WAIT_OBJECT_0 :
				// Got the restart event
				signaled = true;
			break;
		}
	}
}

void thread::waitjoiningcancel (thread* thread)
{
	// Wait for joining thread or cancel
	// Warning: thread->guard *must* have been locked before calling this function.
	//          Works only if this == self thread.

	DWORD tec;
	this->testcancel();
	if (thread->state < thread::terminal)
	{
		lisle::Acquirer self(this->guard);
		thread->joiner = this;
		if (this->cancel.state == thread::cancel::enable)
			this->cancel.joining = thread;
		{
			lisle::Releaser release(thread->guard);
			this->waitrestart();
		}
		this->cancel.joining = NULL;
		thread->joiner = NULL;
	}
	else // thread is waiting on us to restart it to perform join
		thread->restart();
	this->testcancel();
	GetExitCodeThread(thread->handle, &tec);
}

}
