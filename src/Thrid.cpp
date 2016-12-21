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
#include <lisle/Thrid>
#include <lisle/Acquirer>
#include <lisle/Releaser>
#include <lisle/assert>
#include <lisle/self>
#include "prioqueue.h"

#ifdef _MSC_VER
#pragma warning (disable:4290)
#endif

namespace lisle {

// We don't export lisle::inter::self in API.
// This is why this constructor is not in include/lisle/Thread.Id.
Thrid::Thrid ()
: thr(self)
{}

// Don't move in include/lisle/Thread.Id.
Thrid::Thrid (const threadle& thr)
: thr(thr)
{
	// Only for Thread::create, else it's private (i.e: unused).
}

void Thrid::detach ()
throw (lisle::detach)
{
	Acquirer thread(thr->guard);
	assert(thr->state < thread::canceling, lisle::detach(detach::missing));
	assert(thr->detach.state != thread::detach::detached, lisle::detach(detach::detached));
	if (thr->joiner == NULL)
		thr->detach.state = thread::detach::detached;
}

void Thrid::cancel ()
throw ()
{
	Acquirer thread(thr->guard);
	if ((thr->cancel.state == thread::cancel::enable) && (thr->state < thread::canceling))
	{
		thr->cancel.pending = true;
		// If thr is waiting in join() then cleanup and restart thr.
		// Note: If we can cancel then we are not joining. Thus no self check.
		if (thr->cancel.joining != NULL)
		{
			thr->cancel.joining->joiner = NULL;
			thr->cancel.joining = NULL;
			thr->restart();
		}
		// If thr is in a waiting queue then restart it so it can see that it was canceled and throw the cancel exception.
		if (thr->cancel.waiting.queue != NULL)
		{
			bool restart;
			{
				Acquirer device(*thr->cancel.waiting.guard);
				prioqueue waitqueue(&thr->cancel.waiting.queue);
				restart = waitqueue.remove(thr);
			}
			if (restart)
				thr->restart();
		}
	}
}

void Thrid::resume ()
throw ()
{
	Acquirer thread(thr->guard);
	if (thr->state == thread::suspended)
	{
		thr->resume();
		thr->state = thread::running;
	}
}

Exit Thrid::ijoin ()
throw (lisle::join, lisle::deadlock, lisle::thrcancel)
{
	threadle self(lisle::self);
	Exit result;
	Acquirer object(thr->guard);
	assert(!thr->equal(self), lisle::deadlock());
	assert(thr->state != thread::joined, lisle::join(join::missing));
	assert(thr->detach.state == thread::detach::joinable, lisle::join(join::detached));
	assert(thr->joiner == NULL, lisle::join(join::duplicate));
	self->waitjoiningcancel(thr);
	// Never reached if canceled
	switch (thr->state)
	{
		default :
		case thread::exceptioned :
			result = exceptioned;
			break;
		case thread::canceled :
			result = canceled;
			break;
		case thread::terminated :
			result = terminated;
			break;
	}
	thr->state = thread::joined;
	return result;
}

}
