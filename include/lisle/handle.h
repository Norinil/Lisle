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
// A data handle. Also called smart pointer in literature.
// The data pointed by a handle is controled by a counter. When the counter
// falls to zero the data is returned to the system. This allows to share
// pointers. Since the counter is atomic this data can even be safely shared
// among threads.
//
// Usage:
// Handle<T> handle(new T(init_value));
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/countic.h>
#include <cstdlib>

namespace lisle {
template <typename T> class handle
{
public:
	~handle () { tryclean(); }
	handle (T* data = 0) : data(data), users(new countic(1)) {}
	handle (const handle& that) : data(that.data), users(that.users) { users->inc(); }
	operator T* () { return data; }
	operator T* () const { return data; }
	T* operator -> () { return data; }
	T* operator -> () const { return data; }
	
	handle& operator = (const handle& that)
	{
		if (this != &that)
		{
			that.users->inc();
			this->tryclean();
			this->data = that.data;
			this->users = that.users;
		}
		return *this;
	}

protected:
	T* data;
	countic* users; // pointer because shared among all handles pointing to the same data
	void operator [] (size_t);
	void operator [] (int);
	
private:
	void tryclean ()
	{ // clean if this is the last handle to data
		if (users->dectz())
		{
			if (data) delete data;
			delete users;
		}
	}
};
}
