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
#include <lisle/condition.h>
#include <lisle/acquirer.h>
#include <lisle/releaser.h>
#include <lisle/self.h>
#include "self.h"
#include "assert.h"
#include "prioqueue.h"

#ifdef _MSC_VER
#pragma warning (disable : 4290 4459)
#endif

namespace lisle {

condition::~condition ()
throw (permission)
{
	bool waiters = data.waiting != 0;
	assert(!waiters, permission());
}

condition::condition (mutex& mutex)
throw (resource)
: waitex(mutex)
{}

void condition::signal ()
throw ()
{
	intern::thread* waiting = NULL;
	{
		acquirer acquire(data.guard);
		prioqueue waitqueue(&data.waiting);
		if (!waitqueue.empty())
		{
			waiting = waitqueue.top();
			waitqueue.pop();
		}
	}
	if (waiting != NULL)
		waiting->restart();
}

void condition::broadcast ()
throw ()
{
	intern::thread* waiting;
	{
		acquirer acquire(data.guard);
		waiting = data.waiting;
		data.waiting = NULL;
	}
	if (waiting != NULL)
	{
		prioqueue waitqueue(&waiting);
		while (!waitqueue.empty())
		{
			waitqueue.top()->restart();
			waitqueue.pop();
		}
	}
}

void condition::wait ()
throw (permission, thrcancel)
{
	intern::threadle self(intern::self);
	self->testcancel();
	// Never reached if pending cancellation
	releaser invariant(waitex); // can throw permission if waitex is not locked
	prioqueue waitqueue(&data.waiting);
	{
		acquirer device(data.guard);
		waitqueue.push(self);
	}
	try
	{
		self->waitrestartcancel(data.waiting, data.guard);
		// Never reached if canceled
		// If we were restarted (signaled/broadcasted) then we were poped from the waiting queue.
		// No need to remove self from the waiting queue here.
	}
	catch (thrcancel&)
	{
		waitqueue.remove(self);
		throw;
	}
}

void condition::wait (const duration& span)
throw (permission, timeout, thrcancel)
{
	intern::threadle self(intern::self);
	self->testcancel();
	// Never reached if pending cancellation
	releaser invariant(waitex); // can throw permission if waitex is not locked
	prioqueue waitqueue(&data.waiting);
	{
		acquirer device(data.guard);
		waitqueue.push(self);
	}
	try
	{
		self->waitrestartcancel(span, data.waiting, data.guard);
		// Never reached if canceled or timedout
		// If we were restarted (signaled/broadcasted) then we were poped from the waiting queue.
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

}
