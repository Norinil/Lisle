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
#include <lisle/Condition>
#include <lisle/Acquirer>
#include <lisle/Releaser>
#include <lisle/assert>
#include <lisle/self>
#include "prioqueue.h"

#ifdef _MSC_VER
#pragma warning (disable:4290)
#endif

namespace lisle {

Condition::~Condition ()
throw (permission)
{
	bool waiters = cond.waiting != 0;
	assert(!waiters, permission());
}

Condition::Condition (Mutex& mutex)
throw (resource)
: mutex(mutex)
{}

void Condition::signal ()
throw ()
{
	thread* waiting = NULL;
	{
		Acquirer device(cond.guard);
		prioqueue waitqueue(&cond.waiting);
		if (!waitqueue.empty())
		{
			waiting = waitqueue.top();
			waitqueue.pop();
		}
	}
	if (waiting != NULL)
		waiting->restart();
}

void Condition::broadcast ()
throw ()
{
	thread* waiting;
	{
		Acquirer device(cond.guard);
		waiting = cond.waiting;
		cond.waiting = NULL;
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

void Condition::wait ()
throw (permission, thrcancel)
{
	threadle self(lisle::self);
	self->testcancel();
	// Never reached if pending cancellation
	Releaser invariant(mutex); // can throw permission if mutex is not locked
	prioqueue waitqueue(&cond.waiting);
	{
		Acquirer device(cond.guard);
		waitqueue.push(self);
	}
	try
	{
		self->waitrestartcancel(cond.waiting, cond.guard);
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

void Condition::wait (const Duration& duration)
throw (permission, timeout, thrcancel)
{
	threadle self(lisle::self);
	self->testcancel();
	// Never reached if pending cancellation
	Releaser invariant(mutex); // can throw permission if mutex is not locked
	prioqueue waitqueue(&cond.waiting);
	{
		Acquirer device(cond.guard);
		waitqueue.push(self);
	}
	try
	{
		self->waitrestartcancel(duration, cond.waiting, cond.guard);
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
