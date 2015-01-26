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
#include <lisle/Shorex>
#include <lisle/create>
#include <lisle/Acquirer>
#include <iostream>

#if defined(_MSC_VER) || defined(__BORLANDC__)
#undef max
#endif

using namespace lisle;
using namespace std;

TEST (ShorexTest, construct)
{
	Shorex shorex; // valgrind will see if something is not destroyed
}

/// Destroying a locked shorex must throw a permission exception
TEST (ShorexTest, destroy_locked)
{
	// Ensure a permission exception is thrown when trying to destroy a shorex that is locked
	EXPECT_THROW({Shorex shorex; shorex.lock(exclusive);}, permission);
	EXPECT_THROW({Shorex shorex; shorex.lock(shared);}, permission);
	EXPECT_THROW({Shorex shorex(shared); shorex.lock(shared);}, permission);
	EXPECT_THROW({Shorex shorex(shared); shorex.lock(exclusive);}, permission);
	EXPECT_THROW({Shorex shorex(exclusive); shorex.lock(shared);}, permission);
	EXPECT_THROW({Shorex shorex(exclusive); shorex.lock(exclusive);}, permission);
}

/// Destroying a unlocked shorex should not throw an exception
TEST (ShorexTest, destroy_unlocked)
{
	// Ensure a permission exception is thrown when trying to destroy a shorex that is locked
	EXPECT_NO_THROW({Shorex shorex; shorex.lock(exclusive); shorex.unlock();});
	EXPECT_NO_THROW({Shorex shorex; shorex.lock(shared); shorex.unlock();});
	EXPECT_NO_THROW({Shorex shorex(shared); shorex.lock(shared); shorex.unlock();});
	EXPECT_NO_THROW({Shorex shorex(shared); shorex.lock(exclusive); shorex.unlock();});
	EXPECT_NO_THROW({Shorex shorex(exclusive); shorex.lock(shared); shorex.unlock();});
	EXPECT_NO_THROW({Shorex shorex(exclusive); shorex.lock(exclusive); shorex.unlock();});
}

/// Test I/O device like usage of shorex
namespace shorex {
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
			Acquirer protect(counters);
			readers++;
			if (readers > max.readers) max.readers = readers;
		}
		for (size_t i=0; i<4; ++i)
		{
			cout << ".";
			cout.flush();
			sleep(Duration(0.05)); // simlate reading, gives other threads a chance to run
			{
				Acquirer protect(counters);
				EXPECT_GT(readers, 0);
				EXPECT_EQ(writers, 0);
			}
		}
		{
			Acquirer protect(counters);
			readers--;
		}
		guard.unlock();
	}
	void write ()
	{
		guard.lock(exclusive);
		{
			Acquirer protect(counters);
			writers++;
			if (writers > max.writers) max.writers = writers;
		}
		for (size_t i=0; i<2; ++i)
		{
			cout << "-";
			cout.flush();
			sleep(Duration(0.05)); // simlate writing, gives other threads a chance to run
			{
				Acquirer protect(counters);
				EXPECT_EQ(readers, 0);
				EXPECT_EQ(writers, 1);
			}
		}
		{
			Acquirer protect(counters);
			writers--;
		}
		guard.unlock();
	}
private:
	Shorex guard;
	Mutex counters; // counters mutex to be used in locker
};
void workerreadwrite (Handle<Device> device)
{
	device->read();
	device->write();
}
void workerwriteread (Handle<Device> device)
{
	device->write();
	device->read();
}
void spawnerreadwrite (Handle<Device> device)
{
	Thrid tid[2];
	tid[0] = create(Thread(workerreadwrite, device));
	tid[1] = create(Thread(workerwriteread, device));
	yield();
	tid[0].join();
	tid[1].join();
}
void spawnerwriteread (Handle<Device> device)
{
	Thrid tid[2];
	tid[0] = create(Thread(workerwriteread, device));
	tid[1] = create(Thread(workerreadwrite, device));
	yield();
	tid[0].join();
	tid[1].join();
}
}
TEST (ShorexTest, protectIODevice)
{
	Handle<shorex::Device> device(new shorex::Device);
	Thrid tid[4];
	tid[0] = create(Thread(shorex::workerreadwrite, device));
	tid[1] = create(Thread(shorex::spawnerwriteread, device));
	tid[2] = create(Thread(shorex::workerwriteread, device));
	tid[3] = create(Thread(shorex::spawnerreadwrite, device));
	yield();
	tid[0].join();
	tid[1].join();
	tid[2].join();
	tid[3].join();
	EXPECT_EQ(device->readers, 0);
	EXPECT_EQ(device->writers, 0);
	EXPECT_EQ(device->max.writers, 1);
	EXPECT_GT(device->max.readers, 1);
	cout << endl;
}
