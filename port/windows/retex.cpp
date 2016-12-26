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
#include <lisle/retex.h>
#include <limits.h>
#include <intrin.h>
#include "../../src/assert.h"

#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)

#define OWNER_NONE 0

#ifdef _MSC_VER
#pragma warning (disable:4290)
#pragma warning (disable:4702)
#endif

namespace lisle {

retex::~retex ()
throw (permission)
{
	if (data.event != NULL)
		CloseHandle(data.event);
	assert(data.waiters == 0, permission());
}

retex::retex ()
throw (resource)
{
	data.event = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(data.event != NULL, resource());
	data.waiters = 0;
	data.owner = OWNER_NONE;
	data.recursions = 0;
}

void retex::lock ()
throw ()
{
	DWORD self;
	if (_InterlockedIncrement(&data.waiters) == 1)
		data.owner = GetCurrentThreadId();
	else
	{
		self = GetCurrentThreadId();
		if (data.owner == self)
		{
			_InterlockedDecrement(&data.waiters);
			data.recursions++;
		}
		else
		{
			WaitForSingleObject(data.event, INFINITE);
			data.owner = self;
		}
	}
}

bool retex::trylock ()
throw ()
{
	if (InterlockedCompareExchange(&data.waiters, 1, 0) == 0)
	{
		data.owner = GetCurrentThreadId();
		return true;
	}
	else
	{
		if (data.owner == GetCurrentThreadId())
		{
			data.recursions++;
			return true;
		}
		else
			return false;
	}
}

void retex::unlock ()
throw (permission)
{
	assert(data.owner == GetCurrentThreadId(), permission());
	if (data.recursions == 0)
	{
		data.owner = OWNER_NONE;
		if (data.waiters != 0)
		{
			if (_InterlockedDecrement(&data.waiters) > 0)
				SetEvent(data.event);
		}
	}
	else
		data.recursions--;
}

}
