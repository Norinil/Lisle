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
#include "test.h"
#include <lisle/shorex.h>
#include <lisle/create.h>
#include <lisle/acquirer.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
#undef max
#endif

using namespace lisle;

TEST (Shorex, construct)
{
	shorex soe; // valgrind will see if something is not destroyed
}

/// Destroying a locked shorex must throw a permission exception
TEST (Shorex, destroy_locked)
{
	// Ensure a permission exception is thrown when trying to destroy a shorex that is locked
	EXPECT_THROW({shorex soe; soe.lock(exclusive);}, permission);
	EXPECT_THROW({shorex soe; soe.lock(shared);}, permission);
	EXPECT_THROW({shorex soe(shared); soe.lock(shared);}, permission);
	EXPECT_THROW({shorex soe(shared); soe.lock(exclusive);}, permission);
	EXPECT_THROW({shorex soe(exclusive); soe.lock(shared);}, permission);
	EXPECT_THROW({shorex soe(exclusive); soe.lock(exclusive);}, permission);
}

/// Destroying a unlocked shorex should not throw an exception
TEST (Shorex, destroy_unlocked)
{
	// Ensure a permission exception is thrown when trying to destroy a shorex that is locked
	EXPECT_NO_THROW({shorex soe; soe.lock(exclusive); soe.unlock();});
	EXPECT_NO_THROW({shorex soe; soe.lock(shared); soe.unlock();});
	EXPECT_NO_THROW({shorex soe(shared); soe.lock(shared); soe.unlock();});
	EXPECT_NO_THROW({shorex soe(shared); soe.lock(exclusive); soe.unlock();});
	EXPECT_NO_THROW({shorex soe(exclusive); soe.lock(shared); soe.unlock();});
	EXPECT_NO_THROW({shorex soe(exclusive); soe.lock(exclusive); soe.unlock();});
}

/// Test I/O device like usage of shorex
namespace Shorex {
class Device
{
public:
	int readers;
	int writers;
	struct max
	{
		int readers;
		int writers;
		max () : readers(0), writers(0) {}
	} max;
	Device () : readers(0), writers(0) {}
	void read ()
	{
		guard.lock(shared);
		{
			acquirer protect(counters);
			readers++;
			if (readers > max.readers) max.readers = readers;
		}
		for (size_t i=0; i<4; ++i)
		{
			sleep(duration(0.05)); // simlate reading, gives other threads a chance to run
			{
				acquirer protect(counters);
				EXPECT_GT(readers, 0);
				EXPECT_EQ(writers, 0);
			}
		}
		{
			acquirer protect(counters);
			readers--;
		}
		guard.unlock();
	}
	void write ()
	{
		guard.lock(exclusive);
		{
			acquirer protect(counters);
			writers++;
			if (writers > max.writers) max.writers = writers;
		}
		for (size_t i=0; i<2; ++i)
		{
			sleep(duration(0.05)); // simlate writing, gives other threads a chance to run
			{
				acquirer protect(counters);
				EXPECT_EQ(readers, 0);
				EXPECT_EQ(writers, 1);
			}
		}
		{
			acquirer protect(counters);
			writers--;
		}
		guard.unlock();
	}
private:
	shorex guard;
	mutex counters; // counters mutex to be used in locker
};
void workerreadwrite (handle<Device> device)
{
	device->read();
	device->write();
}
void workerwriteread (handle<Device> device)
{
	device->write();
	device->read();
}
void spawnerreadwrite (handle<Device> device)
{
	thrid tid[2];
	tid[0] = create(thread(workerreadwrite, device));
	tid[1] = create(thread(workerwriteread, device));
	yield();
	tid[0].join();
	tid[1].join();
}
void spawnerwriteread (handle<Device> device)
{
	thrid tid[2];
	tid[0] = create(thread(workerwriteread, device));
	tid[1] = create(thread(workerreadwrite, device));
	yield();
	tid[0].join();
	tid[1].join();
}
}
TEST (Shorex, protectIODevice)
{
	handle<Shorex::Device> device(new Shorex::Device);
	thrid tid[4];
	tid[0] = create(thread(Shorex::workerreadwrite, device));
	tid[1] = create(thread(Shorex::spawnerwriteread, device));
	tid[2] = create(thread(Shorex::workerwriteread, device));
	tid[3] = create(thread(Shorex::spawnerreadwrite, device));
	yield();
	tid[0].join();
	tid[1].join();
	tid[2].join();
	tid[3].join();
	EXPECT_EQ(device->readers, 0);
	EXPECT_EQ(device->writers, 0);
	EXPECT_EQ(device->max.writers, 1);
	EXPECT_GT(device->max.readers, 1);
}
