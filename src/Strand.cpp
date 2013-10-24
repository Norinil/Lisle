/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004-2012, Markus Schütz
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
#include <lisle/Strand>
#include <lisle/Strid>
#include <lisle/Thread>
#include <lisle/create>

#ifdef _MSC_VER
#pragma warning (disable:4290)
#endif

namespace lisle {

void startup (Handle<Strand> object)
{
	object->main();
}

Strid::Strid (Strand* strand)
throw (resource, permission)
: Handle<Strand>(strand)
{
	tid = create(Thread(&startup, *this, strand->mode, strand->state), strand->schedule);
}

Strid::Strid (const Strid& that)
: Handle<Strand>(that)
, tid(that.tid)
{}

Strid& Strid::operator = (const Strid& that)
{
	Handle<Strand>::operator=(that);
	this->tid = that.tid;
	return *this;
}

}
