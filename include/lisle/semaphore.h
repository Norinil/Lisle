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
#pragma once
#include <lisle/mutex.h>
#include <lisle/duration.h>
#include <lisle/exception.h>
#include <cstdint>

#ifdef _MSC_VER
#pragma warning (push,2)
#undef max
#endif

namespace lisle { namespace intern { struct thread; } }

namespace lisle {
class semaphore
{
public:
	~semaphore () throw (permission);
	semaphore (size_t resources = 1) throw (resource, underflow);
	size_t resources () const;
	size_t available () const;
	bool trywait ();
	void wait () throw (thrcancel);
	void wait (const duration& span) throw (timeout, thrcancel);
	void post () throw (overflow);
private:
	struct data
	{
		size_t resources;
		size_t available;
		intern::thread* waiting; // Linked list of waiting threads
		mutex guard;
		data (size_t resources) : resources(resources), available(resources), waiting(0), guard() {}
	} data;
	semaphore (const semaphore&);
	semaphore& operator = (const semaphore&);
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
