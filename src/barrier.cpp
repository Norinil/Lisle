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
#include <lisle/barrier.h>
#include <lisle/acquirer.h>
#include <lisle/self.h>
#include "self.h"
#include "assert.h"
#include "prioqueue.h"

#ifdef _MSC_VER
#pragma warning (disable : 4290 4459)
#endif

namespace lisle {

barrier::~barrier ()
throw (permission)
{
	size_t waiters = data.waiting.count;
	assert(waiters == 0, permission());
}

barrier::barrier (size_t clients)
throw (resource)
: data(clients)
{}

void barrier::wait ()
throw ()
{
	intern::thread* waiting = NULL;
	intern::threadle self(intern::self);
	bool wait = false;
	{
		acquirer device(data.guard);
		data.waiting.count++;
		if (data.waiting.count == data.height)
		{
			data.waiting.count = 0;
			waiting = data.waiting.queue;
			data.waiting.queue = NULL;
		}
		else
		{
			prioqueue waitqueue(&data.waiting.queue);
			waitqueue.push(self);
			wait = true;
		}
	}
	if (wait)
	{
		acquirer thread(self->guard);
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

size_t barrier::height () const
{
	return data.height;
}

size_t barrier::size () const
{
	return data.height;
}

}
