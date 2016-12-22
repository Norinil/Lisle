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
#include <lisle/thrid.h>
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

// We don't export lisle::inter::self in API.
// This is why this constructor is not in include/lisle/Thread.Id.
thrid::thrid ()
: thr(intern::self)
{}

// Don't move in include/lisle/Thread.Id.
thrid::thrid (const intern::threadle& thr)
: thr(thr)
{
	// Only for Thread::create, else it's private (i.e: unused).
}

void thrid::detach ()
throw (lisle::detach)
{
	acquirer thread(thr->guard);
	assert(thr->state < intern::thread::canceling, lisle::detach(detach::missing));
	assert(thr->detach.state != intern::thread::detach::detached, lisle::detach(detach::detached));
	if (thr->joiner == NULL)
		thr->detach.state = intern::thread::detach::detached;
}

void thrid::cancel ()
throw ()
{
	acquirer thread(thr->guard);
	if ((thr->cancel.state == intern::thread::cancel::enable) && (thr->state < intern::thread::canceling))
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
				acquirer device(*thr->cancel.waiting.guard);
				prioqueue waitqueue(&thr->cancel.waiting.queue);
				restart = waitqueue.remove(thr);
			}
			if (restart)
				thr->restart();
		}
	}
}

void thrid::resume ()
throw ()
{
	acquirer thread(thr->guard);
	if (thr->state == intern::thread::suspended)
	{
		thr->resume();
		thr->state = intern::thread::running;
	}
}

Exit thrid::ijoin ()
throw (lisle::join, lisle::deadlock, lisle::thrcancel)
{
	intern::threadle self(intern::self);
	Exit result;
	acquirer object(thr->guard);
	assert(!thr->equal(self), lisle::deadlock());
	assert(thr->state != intern::thread::joined, lisle::join(join::missing));
	assert(thr->detach.state == intern::thread::detach::joinable, lisle::join(join::detached));
	assert(thr->joiner == NULL, lisle::join(join::duplicate));
	self->waitjoiningcancel(thr);
	// Never reached if canceled
	switch (thr->state)
	{
		default :
		case intern::thread::exceptioned :
			result = exceptioned;
			break;
		case intern::thread::canceled :
			result = canceled;
			break;
		case intern::thread::terminated :
			result = terminated;
			break;
	}
	thr->state = intern::thread::joined;
	return result;
}

}
