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
// Description:
// Atomic counter implementation for x86 gcc:
// - Intel 32-64 bit family (x86, x86_64)
// - GNU compiler collection
//
// Source:
// This code was inspired from the Linux kernel/include/asm-i386/atomic.h
// Thanks to the anonymous author (not mentioned in the atomic.h file)
//
///////////////////////////////////////////////////////////////////////////////
*/
#include <lisle/Countic>

namespace lisle {

void Countic::inc ()
{
	__asm__ __volatile__
	(
		"lock; incl %0"
		:"=m"(this->val)
		:"m"(this->val)
	);
}

void Countic::dec ()
{
	__asm__ __volatile__
	(
		"lock; decl %0"
		:"=m"(this->val)
		:"m"(this->val)
	);
}

bool Countic::inctz ()
{
	unsigned char c;
	__asm__ __volatile__
	(
		"lock; incl %0; setz %1"
		:"=m"(this->val), "=qm"(c)
		:"m"(this->val)
		:"memory"
	);
	return c != 0;
}

bool Countic::dectz ()
{
	unsigned char c;
	__asm__ __volatile__
	(
		"lock; decl %0; setz %1"
		:"=m"(this->val), "=qm"(c)
		:"m"(this->val)
		:"memory"
	);
	return c != 0;
}

}
