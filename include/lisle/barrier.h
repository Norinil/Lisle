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
class barrier
{
public:
	~barrier () throw (permission);
	barrier (size_t clients) throw (resource);
	size_t height () const;
	size_t size () const;
	void wait () throw ();
private:
	struct data {
		struct waiting
		{
			size_t count;
			intern::thread* queue;
			waiting () : count(0), queue(0) {}
		} waiting;
		size_t height;
		mutex guard;
		data (size_t size) : height(size) {}
		void wait ();
	} data;
	barrier (const barrier&);
	barrier& operator = (const barrier&);
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
