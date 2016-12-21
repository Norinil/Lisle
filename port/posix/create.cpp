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
//
// Posix implementation of the Lisle thread factory.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include <lisle/create>
#include <lisle/self>
#include <lisle/thrargs>
#include <lisle/Acquirer>
#include <cerrno>
#include <cstdio>
#include <new>
#include "../../src/assert.h"

static int convert (const lisle::Schedule::Priority& priority, int policy)
{
	int result;
	int min, max;
	int sysrange, lislerange;
	min = sched_get_priority_min(policy);
	max = sched_get_priority_max(policy);
	sysrange = max - min;
	lislerange = lisle::Schedule::Critical - lisle::Schedule::Lazy;
	result = min + (priority * sysrange / lislerange);
	if (result < min) result = min;
	if (result > max) result = max;
	return result;
}

static void convert (const lisle::Schedule& sched, pthread_attr_t& attr)
throw (lisle::permission)
{
	int policy;
	sched_param param;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (sched.inherit())
		pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
	else
	{
		pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
		switch (sched.policy())
		{
			case lisle::Schedule::Regular :
				policy = SCHED_OTHER;
				break;
			case lisle::Schedule::Realtime :
				policy = SCHED_RR;
				break;
			default :
				// Internal error. Must be fixed. Throw an unexpected exception.
				printf("Lisle internal error: %s, line %u\n", __FILE__, __LINE__);
				throw false;
		}
		assert(pthread_attr_setschedpolicy(&attr, policy) != ENOTSUP, lisle::permission());
		param.sched_priority = convert(sched.priority(), policy);
		pthread_attr_setschedparam(&attr, &param);
	}
}

static void* setup (void* args)
{
	// Spawned thread
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	lisle::startup((lisle::thrargs*)args);
	pthread_exit(NULL);
}

namespace lisle {
Thrid create (const Thread& start, const Schedule& schedule)
throw (resource, permission, virthread)
{
	// Calling thread
	try
	{
		Thrid result(new thread); // Can throw std::bad_alloc
		int rc;
		pthread_attr_t tattr;
		thrargs* targs;
		sched_param param;
		
		assert(start.main() != NULL, lisle::virthread());
		targs = new thrargs(result.thr); // Can throw std::bad_alloc
		{
			Acquirer lock(targs->guard);
			targs->start = start;
			convert(schedule, tattr); // Can throw permission exception
			pthread_attr_getschedparam(&tattr, &param);
			result.thr->priority = param.sched_priority;
			rc = pthread_create(&result.thr->handle, &tattr, &setup, targs);
			if (rc != 0)
			{
				delete targs;
				throw lisle::resource();
			}
			pthread_attr_destroy(&tattr);
			targs->copied.wait();
		}
		delete targs;
		if (start.mode() == Thread::Suspended)
		{
			while (result.thr->state != thread::suspended)
				yield();
		}
		return result;
	}
	catch (std::bad_alloc&)
	{
		throw lisle::resource();
	}
}
}
