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
#include <lisle/sys/thread.h>
#include <lisle/exception.h>
#include <cerrno>

namespace lisle {
namespace sys {

thread::~thread ()
{
	int rc = pthread_cond_destroy(&restart.condition);
	if (rc == EBUSY)
		throw permission();
}

thread::thread ()
: signal(0)
, handle(pthread_self())
{
	int rc;
	pthread_condattr_t attr;
	rc = pthread_condattr_init(&attr);
	if (rc != 0)
		throw resource();
	rc = pthread_cond_init(&restart.condition, &attr);
	pthread_condattr_destroy(&attr);
	if (rc != 0)
		throw resource();
}

uint32_t thread::id () const
{
	return static_cast<uint32_t>(handle);
}

bool thread::equal (const thread& that) const
{
	return pthread_equal(this->handle, that.handle);
}

bool thread::equal (const thread* that) const
{
	return pthread_equal(this->handle, that->handle);
}
	
}
}
