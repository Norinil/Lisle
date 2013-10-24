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
// Posix implementation of thread specific data.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include <lisle/exceptions>
#include <lisle/assert>

namespace lisle {
namespace sys {
namespace local {
template <typename T> static inline void clean (void* data)
{
	// We are not called if data is NULL as stated in the POSIX specifications.
	// Thus no NULL test needed before delete.
	delete (T*)data;
}
}
}
}

namespace lisle {

template <typename T> inline Local<T>::~Local ()
{
	register T* data;
	data = (T*)pthread_getspecific(key);
	if (data)
		delete data;
	pthread_key_delete(key);
}

template <typename T> inline Local<T>::Local ()
throw (resource)
{
	register int rc;
	rc = pthread_key_create(&key, &sys::local::clean<T>);
	assert(rc == 0, resource());
}

template <typename T> inline T& Local<T>::operator = (const T& that)
throw (resource)
{
	register T* data;
	try
	{
		data = (T*)pthread_getspecific(key);
		if (!data) // First time use ?
		{
			data = new T;
			pthread_setspecific(key, data);
		}
		*data = that;
		return *data;
	}
	catch (std::bad_alloc&)
	{
		throw resource();
	}
}

template <typename T> inline Local<T>::operator T* ()
throw ()
{
	register T* data;
	data = (T*)pthread_getspecific(key);
	return data;
}

template <typename T> inline Local<T>::operator T* () const
throw ()
{
	register T* data;
	data = (T*)pthread_getspecific(key);
	return data;
}

template <typename T> inline T* Local<T>::operator -> ()
throw ()
{
	register T* data;
	data = (T*)pthread_getspecific(key);
	return data;
}

template <typename T> inline T* Local<T>::operator -> () const
throw ()
{
	register T* data;
	data = (T*)pthread_getspecific(key);
	return data;
}

}
