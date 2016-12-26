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
// Recursive mutex.
// A mutex that can be recursively locked by the same owner.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/sys/mutex.h>
#include <lisle/exception.h>

#ifdef _MSC_VER
#pragma warning (push,2)
#endif

namespace lisle {
class retex
{
public:
	~retex () throw (permission);
	retex () throw (resource);
	bool trylock () throw ();
	void lock () throw ();
	void unlock () throw (permission);
	bool tryacquire () throw () { return trylock(); }
	void acquire () throw () { lock(); }
	void release () throw (permission) { unlock(); }
private:
	sys::mutex data;
	retex (const retex&);
	retex& operator = (const retex&);
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
