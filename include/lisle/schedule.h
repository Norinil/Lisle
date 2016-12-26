/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2013, Markus Schütz
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

namespace lisle {
class schedule
{
public:
	enum Policy { regular, realtime };
	enum Priority { lazy, lowest, low, normal, high, highest, critical };
	~schedule () {}
	schedule () : data() {}
	schedule (Policy policy, Priority priority) : data(policy, priority) {}
	schedule (const schedule& that) : data(that.data) {}
	schedule& operator = (const schedule& that) { this->data = that.data; return *this; }
	bool inherit () const { return data.inherit; }
	Policy policy () const { return data.policy; }
	Priority priority () const { return data.priority; }
	void set (Policy policy, Priority priority) { data.inherit = false; data.policy = policy, data.priority = priority; }
private:
	struct data
	{
		bool inherit;
		Policy policy;
		Priority priority;
		data () : inherit(true) {}
		data (Policy policy, Priority priority) : inherit(false), policy(policy), priority(priority) {}
	} data;
};
}
