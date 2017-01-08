/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005-2012, Markus Schütz
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
#include <lisle/semaphore.h>
#include <lisle/acquirer.h>
#include <lisle/self.h>
#include "self.h"
#include <limits>
#include "assert.h"
#include "prioqueue.h"

#ifdef _MSC_VER
#pragma warning (disable : 4290 4459)
#undef max
#endif

using namespace std;

namespace lisle {

semaphore::~semaphore ()
throw (lisle::permission)
{
	bool waiters = data.waiting != 0;
	assert(!waiters, lisle::permission());
}

semaphore::semaphore (size_t resources)
throw (resource, underflow)
: data(resources)
{
	if (resources < 1)
		throw underflow();
}

size_t semaphore::resources () const
{
	return data.resources;
}

size_t semaphore::available () const
{
	return data.available;
}

bool semaphore::trywait ()
{
	if (data.available > 0)
	{
		wait();
		return true;
	}
	else
		return false;
}

void semaphore::wait ()
throw (thrcancel)
{
	intern::threadle self(intern::self);
	self->testcancel();
	// Never reached if pending cancellation
	prioqueue waitqueue(&data.waiting);
	{
		acquirer acquire(data.guard);
		if (data.available > 0)
		{
			data.available--;
			return; // no wait since the semaphore has available resources
		}
		else
			waitqueue.push(self);
	}
	try
	{
		self->waitrestartcancel(data.waiting, data.guard);
		// Never reached if canceled
		// If we were restarted (posted) then we were poped from the waiting queue.
		// No need to remove self from the waiting queue here.
	}
	catch (thrcancel&)
	{
		waitqueue.remove(self);
		throw;
	}
}

void semaphore::wait (const duration& span)
throw (timeout, thrcancel)
{
	intern::threadle self(intern::self);
	self->testcancel();
	// Never reached if pending cancellation
	prioqueue waitqueue(&data.waiting);
	{
		acquirer acquire(data.guard);
		if (data.available > 0)
		{
			data.available--;
			return; // no wait since the semaphore has available resources
		}
		else
			waitqueue.push(self);
	}
	try
	{
		self->waitrestartcancel(span, data.waiting, data.guard);
		// Never reached if canceled or timedout
		// If we were restarted (posted) then we were poped from the waiting queue.
		// No need to remove self from the waiting queue here.
	}
	catch (timeout&)
	{
		waitqueue.remove(self);
		throw;
	}
	catch (thrcancel&)
	{
		waitqueue.remove(self);
		throw;
	}
}

void semaphore::post ()
throw (overflow)
{
	intern::thread* waiting = 0;
	{
		acquirer acquire(data.guard);
		prioqueue waitqueue(&data.waiting);
		if (!waitqueue.empty())
		{
			waiting = waitqueue.top();
			waitqueue.pop();
		}
		else
		{
			assert(data.available < data.resources, overflow());
			data.available++;
		}
	}
	if (waiting != 0)
		waiting->restart();
}

}
