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
//
// Synchronization device (condition variable) that allows threads to suspend
// execution and relinquish the processors until some predicate or shared 
// data is satisfied.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/mutex.h>
#include <lisle/duration.h>
#include <lisle/exception.h>

#ifdef _MSC_VER
#pragma warning (push,2)
#endif

namespace lisle { namespace intern { struct thread; } }

namespace lisle {
class condition
{
public:
	~condition () throw (permission);
	condition (mutex& mutex) throw (resource);
	void signal () throw ();
	void broadcast () throw ();
	void wait () throw (permission, thrcancel);
	void wait (const duration& span) throw (permission, timeout, thrcancel);
	void notify () throw () { this->signal(); }
private:
	struct data
	{
		intern::thread* waiting; // Linked list of waiting threads
		mutex guard;
		data () : waiting(0) {}
	} data;
	mutex& waitex; // don't use as guard, used only in wait methods
	condition (const condition&);
	condition& operator = (const condition&);
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
