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
#include <lisle/Retex>
#include <lisle/assert>
#include <cerrno>

namespace lisle {

Retex::~Retex ()
throw (permission)
{
	int rc = pthread_mutex_destroy(&mutex);
	assert(rc != EBUSY, permission());
}

Retex::Retex ()
throw (resource)
{
	int rc;
	pthread_mutexattr_t attr;
	rc = pthread_mutexattr_init(&attr);
	assert(rc != ENOMEM, resource());
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	rc = pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);
	assert((rc != ENOMEM) && (rc != EAGAIN), resource());
}

void Retex::lock ()
throw ()
{
	int rc = pthread_mutex_lock(&mutex);
}

bool Retex::trylock ()
throw ()
{
	int rc = pthread_mutex_trylock(&mutex);
	return rc != EBUSY;
}

void Retex::unlock ()
throw (permission)
{
	int rc = pthread_mutex_unlock(&mutex);
	assert(rc != EPERM, permission());
}

}
