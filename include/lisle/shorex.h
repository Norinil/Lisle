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
// SHared OR EXclusive access device.
// Also known as rwlock.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/mutex.h>
#include <lisle/exception.h>
#include <cstdint>

#ifdef _MSC_VER
#pragma warning (push,2)
#endif

namespace lisle { namespace intern { struct thread; } }

namespace lisle {
enum access { shared, exclusive };
class shorex
{
public:
	~shorex () throw (permission);
	shorex (access priority = exclusive) throw (resource);
	bool trylock (access access) throw ();
	void lock (access access) throw (deadlock);
	void unlock () throw (permission);
	bool tryacquire (access access) throw () { return trylock(access); }
	void acquire (access access) throw (deadlock) { return lock(access); }
	void release () throw (permission) { return unlock(); }
private:
	struct data
	{
		access priority;
		struct waiting
		{
			intern::thread* shared;
			intern::thread* exclusive;
			waiting () : shared(0), exclusive(0) {}
		} waiting;
		struct shared
		{
			size_t count; // Number of shared access
			intern::thread* owner; // Shared owner thread list head
			shared () : count(0), owner(0) {}
		} shared;
		intern::thread* exclusive; // Identity of exclusive accessor (NULL if none)
		mutex guard;
		~data ();
		data (access priority) : priority(priority), exclusive(0) {}
		bool trylockshared ();
		bool trylockexclusive ();
		void lockshared ();
		void lockexclusive ();
		void unlock ();
	private:
		bool canlockshared ();
	} soe;
	shorex (const shorex&);
	shorex& operator = (const shorex&);
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
