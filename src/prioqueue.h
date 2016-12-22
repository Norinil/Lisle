/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005-2012, Markus Schütz
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
#include <lisle/self>
#include <lisle/EXI>

namespace lisle {
class prioqueue
{
public:
	prioqueue (thread** top) : queue(top) {}
	bool empty () const { return *queue == 0; }
	thread* top () const { return *queue; }
	EXI void push (thread* item);
	EXI void pop ();
	EXI bool remove (thread* item);
private:
	thread** queue;
	prioqueue (const prioqueue&);
	prioqueue& operator = (const prioqueue&);
};
}
