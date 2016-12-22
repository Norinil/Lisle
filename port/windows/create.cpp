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
#include <lisle/create.h>
#include <lisle/self.h>
#include <lisle/acquirer.h>
#include <lisle/schedule.h>
#include <cerrno>
#include <cstdio>
#include <process.h>
#include <new>
#include "../../src/assert.h"
#include "../../src/thrargs.h"

#ifdef _MSC_VER
#pragma warning (disable:4290)
#endif

static int setsched (HANDLE thread, const lisle::schedule& sched)
throw (lisle::permission)
{
	// Returns thread priority

	int policy;
	int priority;
	HANDLE process;

	process = GetCurrentProcess();
	if (sched.inherit())
	{
		SetPriorityClass(process, GetPriorityClass(GetCurrentProcess));
		SetThreadPriority(thread, GetThreadPriority(GetCurrentThread));
		priority = GetThreadPriority(thread);
	}
	else
	{
		switch (sched.policy())
		{
			case lisle::schedule::regular :
				policy = NORMAL_PRIORITY_CLASS;
				break;
			case lisle::schedule::realtime :
				policy = REALTIME_PRIORITY_CLASS;
				break;
			default :
				// Internal error. Must be fixed. Throw an unexpected exception.
				printf("Lisle internal error: %s, line %u\n", __FILE__, __LINE__);
				throw false;
		}
		switch (sched.priority())
		{
			case lisle::schedule::critical :
				priority = THREAD_PRIORITY_TIME_CRITICAL;
				break;
			case lisle::schedule::highest :
				priority = THREAD_PRIORITY_HIGHEST;
				break;
			case lisle::schedule::high :
				priority = THREAD_PRIORITY_ABOVE_NORMAL;
				break;
			case lisle::schedule::normal :
				priority = THREAD_PRIORITY_NORMAL;
				break;
			case lisle::schedule::low :
				priority = THREAD_PRIORITY_BELOW_NORMAL;
				break;
			case lisle::schedule::lowest :
				priority = THREAD_PRIORITY_LOWEST;
				break;
			case lisle::schedule::lazy :
				priority = THREAD_PRIORITY_IDLE;
				break;
			default :
				// Internal error. Must be fixed. Throw an unexpected exception.
				printf("Lisle internal error: %s, line %u\n", __FILE__, __LINE__);
				throw false;
		}
		assert(SetPriorityClass(process, policy) != 0, lisle::permission());
		assert(SetThreadPriority(process, priority) != 0, lisle::permission());
	}
	return priority;
}

static unsigned __stdcall setup (void* args)
{
	// Spawned thread
	// Do not call _endthreadex explicitly, else the stack of local variables is
	// not deleted properly (memory leaks). Especially annoying for self, since
	// it is a handle (sys_thread_t) with users counter and auto-deallocation of
	// associated thread id pointer (sys_thread_t_*) when the users count falls
	// to zero.
	
	lisle::startup((lisle::thrargs*)args);
	return 0;
}

namespace lisle {
thrid create (const thread& start, const schedule& sched)
throw (resource, permission, virthread)
{
	// Calling thread
	// The main reason why we start the thread in suspended mode is that we need
	// the started thread's handle to set its priority. But we also need to set
	// this priority before the started thread begins its actual execution.
	// Creating the thread in suspended mode is the solution that satisfies the
	// above conditions.

	using namespace std;
	try
	{
		thrid result(new intern::thread);
		thrargs* targs;
		assert(start.main() != NULL, lisle::virthread());
		targs = new thrargs(result.thr);
		assert(targs != NULL, lisle::resource());
		result.thr->handle =
			(HANDLE)_beginthreadex(NULL,               // security
			                       0,                  // stack size
			                       &setup,             // start address
			                       targs,              // start fct arg list
			                       CREATE_SUSPENDED,   // 0 | CREATE_SUSPENDED
			        (unsigned int*)&result.thr->nid); // thread identifier
		if (result.thr->handle == 0)
		{
			delete targs;
			switch (errno)
			{
				case EAGAIN :
					throw lisle::resource();
				break;
				default :
					printf("Lisle internal error: %s, line %u\n", __FILE__, __LINE__);
					throw errno;
				break;
			}
		}
		result.thr->state = intern::thread::initial;
		switch (start.state())
		{
			case thread::joinable :
				result.thr->detach.state = intern::thread::detach::joinable;
				break;
			case thread::detached :
				result.thr->detach.state = intern::thread::detach::detached;
				break;
		}
		try
		{
			result.thr->priority = setsched(result.thr->handle, sched);
		}
		catch (lisle::permission)
		{
			delete targs;
			throw;
		}
		// Never reached if setsched threw Permission
		{
			acquirer lock(targs->guard);
			targs->start = start;
			ResumeThread(result.thr->handle);
			targs->copied.wait();
		}
		delete targs;
		if (start.mode() == thread::suspended)
		{
			while (result.thr->state != intern::thread::suspended)
				yield();
		}
		return result;
	}
	catch (std::bad_alloc)
	{
		throw lisle::resource();
	}
}
}
