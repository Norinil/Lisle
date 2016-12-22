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
*/
#include <lisle/retex.h>
#include <cerrno>
#include "../../src/assert.h"

namespace lisle {

retex::~retex ()
throw (permission)
{
	int rc = pthread_mutex_destroy(&data);
	assert(rc != EBUSY, permission());
}

retex::retex ()
throw (resource)
{
	int rc;
	pthread_mutexattr_t attr;
	rc = pthread_mutexattr_init(&attr);
	assert(rc != ENOMEM, resource());
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	rc = pthread_mutex_init(&data, &attr);
	pthread_mutexattr_destroy(&attr);
	assert((rc != ENOMEM) && (rc != EAGAIN), resource());
}

void retex::lock ()
throw ()
{
	int rc = pthread_mutex_lock(&data);
}

bool retex::trylock ()
throw ()
{
	int rc = pthread_mutex_trylock(&data);
	return rc != EBUSY;
}

void retex::unlock ()
throw (permission)
{
	int rc = pthread_mutex_unlock(&data);
	assert(rc != EPERM, permission());
}

}
