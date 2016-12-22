/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2012-2013, Markus Schütz
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
#include <lisle/strand.h>
#include <lisle/handle.h>

#ifdef _MSC_VER
#pragma warning (push,2)
#endif

namespace lisle {
class strid : private handle<strand>
{
public:
	~strid () {}
	strid (strand* strand) throw (resource, permission);
	strid (const strid& that);
	strid& operator = (const strid& that);
	operator uint32_t () const { return tid; }
	operator strand* () const { return handle<strand>::operator strand* (); }
	void detach () throw (lisle::detach) { tid.detach(); }
	void cancel () throw () { tid.cancel(); }
	void resume () throw () { tid.resume(); }
	Exit join () throw (lisle::join, lisle::deadlock, lisle::thrcancel) { return tid.join(); }
private:
	thrid tid;
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
