/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2003-2012, Markus Schütz
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
#include <cstdint>

namespace lisle {
class duration
{
public:
	~duration () {}
	duration () {}
	duration (const duration& that) : data(that.data) {}
	duration& operator = (const duration& that) { this->data = that.data; return *this; }
	duration (double seconds);
	int64_t sec () const { return data.sec; }
	uint32_t nsec () const { return data.nsec; }
	duration operator - () const;
	bool operator == (const duration& that) const;
	bool operator != (const duration& that) const;
	bool operator < (const duration& that) const;
	bool operator > (const duration& that) const;
	bool operator <= (const duration& that) const { return !(*this > that); }
	bool operator >= (const duration& that) const { return !(*this < that); }
	duration& operator += (const duration& that);
	duration& operator -= (const duration& that);
private:
	struct data {
		int64_t sec;
		uint32_t nsec;
		data () : sec(0), nsec(0) {}
		data (const data& that) : sec(that.sec), nsec(that.nsec) {}
	} data;
};
}

lisle::duration operator + (const lisle::duration& lho, const lisle::duration& rho);
lisle::duration operator - (const lisle::duration& lho, const lisle::duration& rho);
