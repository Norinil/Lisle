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
#include <lisle/Barrier>
#include <lisle/Acquirer>
#include <lisle/assert>
#include <lisle/prioqueue>
#include <lisle/self>

#ifdef _MSC_VER
#pragma warning (disable:4290)
#endif

namespace lisle {

Barrier::~Barrier ()
throw (permission)
{
	size_t waiters = barrier.waiting.count;
	assert(waiters == 0, permission());
}

Barrier::Barrier (size_t clients)
throw (resource)
: barrier(clients)
{}

void Barrier::wait ()
throw ()
{
	thread* waiting = NULL;
	threadle self(lisle::self);
	bool wait = false;
	{
		Acquirer device(barrier.guard);
		barrier.waiting.count++;
		if (barrier.waiting.count == barrier.height)
		{
			barrier.waiting.count = 0;
			waiting = barrier.waiting.queue;
			barrier.waiting.queue = NULL;
		}
		else
		{
			prioqueue waitqueue(&barrier.waiting.queue);
			waitqueue.push(self);
			wait = true;
		}
	}
	if (wait)
	{
		Acquirer thread(self->guard);
		self->waitrestart();
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

}
