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
// Synchronization device (event variable) that allows threads to suspend
// execution and relinquish the processors until the device becomes signaled.
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
class event
{
public:
	~event () throw (permission);
	event () throw (resource);
	void signal () throw ();
	void broadcast () throw ();
	void wait () throw (thrcancel);
	void wait (const duration& span) throw (timeout, thrcancel);
	void notify () throw () { this->signal(); }
private:
	struct data
	{
		intern::thread* waiting; // Linked list of waiting threads
		mutex guard;
		data () : waiting(0) {}
	} data;
	event (const event&);
	event& operator = (const event&);
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
