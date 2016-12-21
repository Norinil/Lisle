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
#include <lisle/Event>
#include <lisle/Acquirer>
#include <lisle/self>
#include "assert.h"
#include "prioqueue.h"

#ifdef _MSC_VER
#pragma warning (disable:4290)
#endif

namespace lisle {

Event::~Event ()
throw (permission)
{
	bool waiters = event.waiting != 0;
	assert(!waiters, permission());
}

Event::Event ()
throw (resource) // the internal event.mutex can throw a resource exception
{}

void Event::signal ()
throw ()
{
	thread* waiting = NULL;
	{
		Acquirer device(event.guard);
		prioqueue waitqueue(&event.waiting);
		if (!waitqueue.empty())
		{
			waiting = waitqueue.top();
			waitqueue.pop();
		}
	}
	if (waiting != NULL)
		waiting->restart();
}

void Event::broadcast ()
throw ()
{
	thread* waiting;
	{
		Acquirer device(event.guard);
		waiting = event.waiting;
		event.waiting = NULL;
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

void Event::wait ()
throw (thrcancel)
{
	threadle self(lisle::self);
	self->testcancel();
	// Never reached if pending cancellation
	prioqueue waitqueue(&event.waiting);
	{
		Acquirer device(event.guard);
		waitqueue.push(self);
	}
	try
	{
		self->waitrestartcancel(event.waiting, event.guard);
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

void Event::wait (const Duration& duration)
throw (timeout, thrcancel)
{
	threadle self(lisle::self);
	self->testcancel();
	// Never reached if pending cancellation
	prioqueue waitqueue(&event.waiting);
	{
		Acquirer device(event.guard);
		waitqueue.push(self);
	}
	try
	{
		self->waitrestartcancel(duration, event.waiting, event.guard);
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
