/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004-2012, Markus Schütz
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
// A thread object base class with an abstract main method that needs to be
// overloaded. This main method is the thread's execution function, the thread
// itself.
// See also Thrandle.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/thread.h>
#include <lisle/thrid.h>
#include <cstdint>

#ifdef _MSC_VER
#pragma warning (push,2)
#endif

namespace lisle { class strid; }

namespace lisle {
class strand
{
	friend class strid;
	friend void startup (handle<strand> object);
public:
	virtual ~strand () {}
	strand (thread::Mode mode = thread::running, thread::State state = thread::joinable, const schedule& schedule = schedule()) : mode(mode), state(state), sched(schedule) {}
	strand (const strand& that) : mode(that.mode), state(that.state), sched(that.sched) {}
	strand& operator = (const strand& that)
	{
		this->mode = that.mode;
		this->state = that.state;
		this->sched = that.sched;
		return *this;
	}
private:
	thread::Mode mode;
	thread::State state;
	schedule sched;
	virtual void main () = 0;
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
