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
// Note:
// For cross platform compatibility, the guaranteed useful range of an atomic
// counter is only 31 bits. The last bit is used for locking on some archi-
// tectures (Sparc for example).
// +----------------------------------------+
// |  signed 31-bit counter value  |  lock  |  Countic
// +----------------------------------------+
//  31                            1     0
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once

namespace lisle {
class countic
{
public:
	countic (long val = 0) : val(val) {}
	operator long () const { return val; }
	void inc ();
	void dec ();
	bool inctz ();
	bool dectz ();
private:
	volatile long val;
	countic (const countic&);
	countic& operator = (const countic&);
};
}
