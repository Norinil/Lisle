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
// Anonymous handle.
// Allows to pass a Handle<T> as a untemplated untyped piece of data but with
// incremented user count to avoid Handle<T>.data deletion.
// The void* equivalent to Handle<T>.
// Used to pass data accross threads during creation, in code that can't be
// templated.
// Should be lisle internal, but it is provided for convenience.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/handle.h>

namespace lisle {
class anondle
{
public:
	~anondle ();
	anondle ();
	anondle (const anondle& that);
	anondle& operator = (const anondle& that);
	template <typename T> anondle (const lisle::handle<T>& that) : data(new lisle::handle<T>(that), link<T>, unlink<T>) {}
	template <typename T> lisle::handle<T>* handle () const { return (lisle::handle<T>*)data.handle; }
private:
	template <typename T> static void* link (void* ptr)
	{
		// create a new user to the handle
		if (ptr)
			return new lisle::handle<T>(*(lisle::handle<T>*)ptr);
		else
			return 0;
	}
	template <typename T> static void unlink (void* ptr)
	{
		// remove a user from handle
		lisle::handle<T>* h = (lisle::handle<T>*)ptr;
		delete h;
	}
	struct data
	{
		void* handle; // actually a pointer to a Handle<T>
		void*(* link)(void*);
		void(* unlink)(void*);
		data () : handle(0), link(0), unlink(0) {}
		data (void* handle, void*(* link)(void*), void(* unlink)(void*)) : handle(handle), link(link), unlink(unlink) {}
	} data;
};
}
