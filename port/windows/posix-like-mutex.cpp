/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2005, Markus Schutz
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
// Win32 non-inlined implementation of Lisle Mutex.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include "lockcmpxchg"
#include <lisle/Mutex>
#include <lisle/Semaphore>
#include <lisle/Assert>
#include <limits.h>

struct Mutex
{
	enum Kind { Normal, Recursive, Detect, Fast=Normal };
	Kind           kind;
	long           waiters;
	DWORD          owner;
	unsigned       recursions;
	sys::Semaphore semaphore;
	sys::Mutex     guard;
};

#define OWNER_NONE 0

#ifdef _MSC_VER
#pragma warning (disable:4290)
#endif

Mutex::~Mutex ()
throw (Exception::Thread::Permission)
{
	if (mutex_.semaphore != NULL)
		CloseHandle(mutex_.semaphore);
	DeleteCriticalSection(&mutex_.guard);
	Assert(mutex_.waiters == 0, Exception::Thread::Permission(GetCurrentThreadId()));
}

Mutex::Mutex (Type type)
throw (Exception::Thread::Resource)
{
	switch (type)
	{
		case Normal :
			mutex_.kind = _lisle_::Mutex::Normal;
		break;
		case Recursive :
			mutex_.kind = _lisle_::Mutex::Recursive;
		break;
		case Detect :
			mutex_.kind = _lisle_::Mutex::Detect;
	}
	try
	{
		InitializeCriticalSection(&mutex_.guard);
	}
	catch (...)
	{
		throw Exception::Thread::Resource(GetCurrentThreadId());
	}
	mutex_.semaphore = CreateSemaphore(NULL, 0, INT_MAX, NULL);
	Assert(mutex_.semaphore != NULL, Exception::Thread::Resource(GetCurrentThreadId()));
	mutex_.waiters = 0;
	mutex_.owner = OWNER_NONE;
	mutex_.recursions = 0;
}

void Mutex::lock ()
throw (Exception::Thread::Deadlock)
{
	DWORD self;

	if (InterlockedIncrement(&mutex_.waiters) == 1)
	{
		if (mutex_.kind != _lisle_::Mutex::Normal)
			mutex_.owner = GetCurrentThreadId();
	}
	else
	{
		self = GetCurrentThreadId();
		if ((mutex_.kind != _lisle_::Mutex::Normal) && (mutex_.owner == self))
		{
			InterlockedDecrement(&mutex_.waiters);
			if (mutex_.kind == _lisle_::Mutex::Detect)
				throw Exception::Thread::Deadlock(self);
			mutex_.recursions++;
		}
		else
		{
			WaitForSingleObject(mutex_.semaphore, INFINITE);
			if (mutex_.kind != _lisle_::Mutex::Normal)
				mutex_.owner = self;
		}
	}
}

bool Mutex::trylock ()
{
	if (_lisle_::lockcmpxchg == NULL)
		_lisle_::lockcmpxchg_init();
	if ((*_lisle_::lockcmpxchg)(&mutex_.waiters, 1, 0) == 0)
	{
		if (mutex_.kind != _lisle_::Mutex::Normal)
			mutex_.owner = GetCurrentThreadId();
		return true;
	}
	else
	{
		if ((mutex_.kind == _lisle_::Mutex::Recursive) &&
		    (mutex_.owner == GetCurrentThreadId()))
		{
			mutex_.recursions++;
			return true;
		}
		else
			return false;
	}
}

void Mutex::unlock ()
throw (Exception::Thread::Permission)
{
	if (mutex_.kind != _lisle_::Mutex::Normal)
		Assert(mutex_.owner == GetCurrentThreadId(), Exception::Thread::Permission(GetCurrentThreadId()));
	if ((mutex_.kind != _lisle_::Mutex::Recursive) || (mutex_.recursions == 0))
	{
		mutex_.owner = OWNER_NONE;
		if (mutex_.waiters != 0)
		{
			EnterCriticalSection(&mutex_.guard);
			if (InterlockedDecrement(&mutex_.waiters) > 0)
				ReleaseSemaphore(mutex_.semaphore, 1, NULL);
			LeaveCriticalSection(&mutex_.guard);
		}
	}
	else
		mutex_.recursions--;
}
