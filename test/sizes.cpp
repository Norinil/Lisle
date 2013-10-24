/* -*- C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005, Markus Schutz
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
#include <cstdio>
#include <semaphore.h>
#include <lisle/sys/interface>
#include <lisle/Thread.Id>
#include <lisle/Thread.Handle>
#include <lisle/Mutex>
#include <lisle/Condition>
#include <lisle/Semaphore>
#include <lisle/Barrier>
#include <lisle/Shorex>
#include <Thread>
#include <ctime>
#include <cstdint>

int main ()
{
	_lisle_::Thread thread;
	printf("Size of uint32_t             = %3u\n", sizeof(uint32_t));
	printf("Size of uint64_t             = %3u\n", sizeof(uint64_t));
	printf("Size of __time_t             = %3u\n", sizeof(__time_t));
	printf("Size of timespec             = %3u\n", sizeof(timespec));
	printf("Size of Duration             = %3u\n", sizeof(Duration));
	printf("Size of pthread_t            = %3u\n", sizeof(pthread_t));
	printf("Size of pthread_mutex_t      = %3u\n", sizeof(pthread_mutex_t));
	printf("Size of pthread_cond_t       = %3u\n", sizeof(pthread_cond_t));
	printf("Size of sem_t                = %3u\n", sizeof(sem_t));
	printf("Size of pthread_barrier_t    = %3u\n", sizeof(pthread_barrier_t));
	printf("Size of pthread_rwlock_t     = %3u\n", sizeof(pthread_rwlock_t));
	printf("Size of Thread::Id           = %3u\n", sizeof(Thread::Id));
	printf("Size of Thread::Handle       = %3u\n", sizeof(Thread::Handle));
	printf("Size of _lisle_::Threadh     = %3u\n", sizeof(_lisle_::Threadh));
	printf("Size of _lisle_::Thread      = %3u\n", sizeof(_lisle_::Thread));
	printf("Size of _lisle_::sys::Thread = %3u\n", sizeof(_lisle_::sys::Thread));
	printf("Size of Mutex                = %3u\n", sizeof(Mutex));
	printf("Size of Condition            = %3u\n", sizeof(Condition));
	printf("Size of Semaphore            = %3u\n", sizeof(Semaphore));
	printf("Size of Barrier              = %3u\n", sizeof(Barrier));
	printf("Size of Shorex               = %3u\n", sizeof(Shorex));
	printf("Size of Args                 = %3u\n", sizeof(Args));
}
