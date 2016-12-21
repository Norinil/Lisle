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
#include <lisle/Semaphore>
#include <lisle/Acquirer>
#include <lisle/assert>
#include <lisle/self>
#include <limits>
#include "prioqueue.h"

#ifdef _MSC_VER
#pragma warning (disable:4290)
#undef max
#endif

using namespace std;

namespace lisle {

size_t Semaphore::max ()
throw ()
{
	return numeric_limits<size_t>::max();
}

Semaphore::~Semaphore ()
throw (lisle::permission)
{
	bool waiters = sem.waiting != 0;
	assert(!waiters, lisle::permission());
}

Semaphore::Semaphore (size_t value)
throw (resource)
: sem(value)
{}

bool Semaphore::trywait ()
throw ()
{
	Acquirer device(sem.guard);
	if (sem.value > 0)
	{
		sem.value--;
		return true;
	}
	else
		return false;
}

void Semaphore::wait ()
throw (thrcancel)
{
	threadle self(lisle::self);
	self->testcancel();
	// Never reached if pending cancellation
	prioqueue waitqueue(&sem.waiting);
	{
		Acquirer device(sem.guard);
		if (sem.value > 0)
		{
			sem.value--;
			return; // no wait since semaphore was > 0
		}
		else
			waitqueue.push(self);
	}
	try
	{
		self->waitrestartcancel(sem.waiting, sem.guard);
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

void Semaphore::post ()
throw (overflow)
{
	thread* waiting = NULL;
	{
		Acquirer device(sem.guard);
		prioqueue waitqueue(&sem.waiting);
		if (!waitqueue.empty())
		{
			waiting = waitqueue.top();
			waitqueue.pop();
		}
		else
		{
			threadle self(lisle::self);
			assert(sem.value < max(), overflow());
			sem.value++;
		}
	}
	if (waiting != NULL)
		waiting->restart();
}

}
