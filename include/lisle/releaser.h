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
// A unlock monitor.
// A class that is constructed with a mutex.
// The mutex is unlocked on construction and locked on destruction.
// Usefull to protect a compound statement.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/mutex.h>

#ifdef _MSC_VER
#pragma warning (push,2)
#endif

namespace lisle {
class releaser
{
public:
	~releaser () throw () { data.lock(); }
	releaser (mutex& mutex) throw (permission) : data(mutex) { data.unlock(); }
private:
	mutex& data;
	releaser (const releaser&);
	releaser& operator = (const releaser&);
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
