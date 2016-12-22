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
#include <lisle/sys/Thread>

namespace lisle {
namespace sys {

Thread::~Thread ()
{
	BOOL rc;
	rc = CloseHandle(restart.event);
	rc = CloseHandle(handle);
}

Thread::Thread ()
{
	handle = GetCurrentThread();
	nid = GetCurrentThreadId();
	restart.event = CreateEvent(NULL, FALSE, FALSE, NULL);
}

uint32_t Thread::id () const
{
	return nid;
}

bool Thread::equal (const Thread& that) const
{
	return this->nid == that.nid;
}

bool Thread::equal (const Thread* that) const
{
	return this->nid == that->nid;
}

}
}
