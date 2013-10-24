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
// Windows implementation of Lisle Retex.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include "lockcmpxchg"
#include <lisle/Retex>
#include <lisle/Semaphore>
#include <lisle/assert>
#include <limits.h>

#define OWNER_NONE 0

#ifdef _MSC_VER
#pragma warning (disable:4290)
#pragma warning (disable:4702)
#endif

namespace lisle {

Retex::~Retex ()
throw (permission)
{
	if (mutex.semaphore != NULL)
		CloseHandle(mutex.semaphore);
	DeleteCriticalSection(&mutex.guard);
	assert(mutex.waiters == 0, permission());
}

Retex::Retex ()
throw (resource)
{
	try
	{
		InitializeCriticalSection(&mutex.guard);
	}
	catch (...)
	{
		throw resource();
	}
	mutex.semaphore = CreateSemaphore(NULL, 0, INT_MAX, NULL);
	assert(mutex.semaphore != NULL, resource());
	mutex.waiters = 0;
	mutex.owner = OWNER_NONE;
	mutex.recursions = 0;
}

void Retex::lock ()
throw ()
{
	DWORD self;
	if (InterlockedIncrement(&mutex.waiters) == 1)
	{
		mutex.owner = GetCurrentThreadId();
	}
	else
	{
		self = GetCurrentThreadId();
		if (mutex.owner == self)
		{
			InterlockedDecrement(&mutex.waiters);
			mutex.recursions++;
		}
		else
		{
			WaitForSingleObject(mutex.semaphore, INFINITE);
			mutex.owner = self;
		}
	}
}

bool Retex::trylock ()
throw ()
{
	if (lockcmpxchg == NULL)
		lockcmpxchg_init();
	if ((*lockcmpxchg)(&mutex.waiters, 1, 0) == 0)
	{
		mutex.owner = GetCurrentThreadId();
		return true;
	}
	else
	{
		if (mutex.owner == GetCurrentThreadId())
		{
			mutex.recursions++;
			return true;
		}
		else
		{
			return false;
		}
	}
}

void Retex::unlock ()
throw (permission)
{
	assert(mutex.owner == GetCurrentThreadId(), permission());
	if (mutex.recursions == 0)
	{
		mutex.owner = OWNER_NONE;
		if (mutex.waiters != 0)
		{
			EnterCriticalSection(&mutex.guard);
			if (InterlockedDecrement(&mutex.waiters) > 0)
				ReleaseSemaphore(mutex.semaphore, 1, NULL);
			LeaveCriticalSection(&mutex.guard);
		}
	}
	else
	{
		mutex.recursions--;
	}
}

}
