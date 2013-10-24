// -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002, Markus Schutz
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
// $Id: Mutex.cpp 3516 2006-07-30 21:57:27Z schutzm $
//
// Project URL: http://lyric.sourceforge.net
//
// Description:
// Darwin non-inlined implementation of Lisle Mutex.
// The functions that implement typed mutexes for Darwin.
// Only Detect and Recursive mutexes are handled here. The Normal type is
// Darwin's default and is in the inlined implementation.
//
///////////////////////////////////////////////////////////////////////////////


#include <lyric/Thread.hpp>
#include <lyric/Assert.hpp>


//===========================================================================//
void lisle_mutex_lock (Mutex::Type type, sys_mutex_t& mutex)
{
	if (pthread_mutex_trylock(&mutex.the) == EBUSY)
		{
			if (pthread_equal(mutex.owner, pthread_self()))
				{
					Assert(type != Mutex::Detect,
					       Exception::Thread::Deadlock(NULL, Thread::Id()));
					mutex.recursions++;
				}
			else
				{
					pthread_mutex_lock(&mutex.the);
					mutex.owner = pthread_self();
				}
		}
	else
		mutex.owner = pthread_self();
}
//---------------------------------------------------------------------------//

//===========================================================================//
bool lisle_mutex_trylock (Mutex::Type type, sys_mutex_t& mutex)
{
	if (pthread_mutex_trylock(&mutex.the) != EBUSY)
		{
			if (pthread_equal(mutex.owner, pthread_self()))
				{
					if (type == Mutex::Recursive)
						mutex.recursions++;
				}
			else
				mutex.owner = pthread_self();
			return true;
		}
	else
		return false;
}
//---------------------------------------------------------------------------//

//===========================================================================//
void lisle_mutex_unlock (Mutex::Type type, sys_mutex_t& mutex)
{
	Assert(pthread_equal(mutex.owner, pthread_self()),
	       Exception::Thread::Permission(NULL, Thread::Id()));
	if (mutex.recursions == 0)
		{
			mutex.owner = NULL;
			pthread_mutex_unlock(&mutex.the);
		}
	else
		mutex.recursions--;
}
//---------------------------------------------------------------------------//

//===========================================================================//
//---------------------------------------------------------------------------//
