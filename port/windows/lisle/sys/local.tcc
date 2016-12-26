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
// Windows implementation of thread specific data.
//
///////////////////////////////////////////////////////////////////////////////
*/

namespace lisle {

template <typename T> inline local<T>::~local ()
{
	register T* data;
	data = (T*)TlsGetValue(key);
	if (data)
		delete data;
	TlsFree(key);
}

template <typename T> inline local<T>::local ()
throw (resource)
{
	key = TlsAlloc();
	if (key == TLS_OUT_OF_INDEXES) throw resource();
	TlsSetValue(key, NULL);
}

template <typename T> inline T& local<T>::operator = (const T& that)
throw (resource)
{
	register T* data;
	try
	{
		data = (T*)TlsGetValue(key);
		if (!data) // First time use ?
		{
			data = new T;
			TlsSetValue(key, data);
		}
		*data = that;
		return *data;
	}
	catch (std::bad_alloc)
	{
		throw resource();
	}
}

template <typename T> inline local<T>::operator T* ()
throw ()
{
	register T* data;
	data = (T*)TlsGetValue(key);
	return data;
}

template <typename T> inline local<T>::operator T* () const
throw ()
{
	register T* data;
	data = (T*)TlsGetValue(key);
	return data;
}

template <typename T> inline T* local<T>::operator -> ()
throw ()
{
	register T* data;
	data = (T*)TlsGetValue(key);
	return data;
}

template <typename T> inline T* local<T>::operator -> () const
throw ()
{
	register T* data;
	data = (T*)TlsGetValue(key);
	return data;
}

}
