/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2014, Markus Schütz
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
// Description:
// Atomic counter implementation for x86-msvc
// - Intel 32 bit family (x86)
// - Microsoft compiler
//
///////////////////////////////////////////////////////////////////////////////
*/
#include <lisle/countic.h>
#include <intrin.h>

#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)

namespace lisle {

void countic::inc ()
{
	_InterlockedIncrement(&this->val);
}

void countic::dec ()
{
	_InterlockedDecrement(&this->val);
}

bool countic::inctz ()
{
	long c = _InterlockedIncrement(&this->val);
	return c == 0;
}

bool countic::dectz ()
{
	long c = _InterlockedDecrement(&this->val);
	return c == 0;
}

}
