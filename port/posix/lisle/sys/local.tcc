/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2017, Markus Schütz
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
#include <lisle/exception.h>

namespace lisle {
namespace sys {
namespace clean {
template <typename T> static inline void local (void* data)
{
	// We are not called if data is NULL as stated in the POSIX specifications.
	// Thus no NULL test needed before delete.
	delete (T*)data;
}
}
}
}

namespace lisle {

template <typename T> inline local<T>::~local ()
{
	T* data;
	data = (T*)pthread_getspecific(key);
	if (data)
		delete data;
	pthread_key_delete(key);
}

template <typename T> inline local<T>::local ()
throw (resource)
{
	int rc;
	rc = pthread_key_create(&key, &sys::clean::local<T>);
	if (rc != 0) throw resource();
}

template <typename T> inline T& local<T>::operator = (const T& that)
throw (resource)
{
	T* data;
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

template <typename T> inline local<T>::operator T* ()
throw ()
{
	T* data;
	data = (T*)pthread_getspecific(key);
	return data;
}

template <typename T> inline local<T>::operator T* () const
throw ()
{
	T* data;
	data = (T*)pthread_getspecific(key);
	return data;
}

template <typename T> inline T* local<T>::operator -> ()
throw ()
{
	T* data;
	data = (T*)pthread_getspecific(key);
	return data;
}

template <typename T> inline T* local<T>::operator -> () const
throw ()
{
	T* data;
	data = (T*)pthread_getspecific(key);
	return data;
}

}
