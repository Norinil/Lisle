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
#include <lisle/barrier.h>
#include <lisle/strid.h>
#include <vector>

using namespace lisle;
using namespace std;

#ifdef _MSC_VER
#pragma warning (disable:4512)
#endif

TEST (Barrier, height_size)
{
	barrier barrier(27);
	EXPECT_EQ(barrier.height(), 27);
	EXPECT_EQ(barrier.size(), 27);
}

TEST (Barrier, sync3workers)
{
	barrier barrier(3);
	class Thread : public strand
	{
	public:
		int hit;
		int pass;
		Thread (lisle::barrier& barrier, const duration& time) : hit(0), pass(0), barrier(barrier), time(time) {}
	private:
		lisle::barrier& barrier;
		duration time; // time spent working (sleeping) between waits at barrier
		void main ()
		{
			for (int i=0; i<3; ++i)
			{
				sleep(time);
				hit++;
				barrier.wait();
				pass++;
			}
		}
	};
	vector<strid> pool;
	pool.push_back(strid(new Thread(barrier, duration(0.01))));
	pool.push_back(strid(new Thread(barrier, duration(0.02))));
	pool.push_back(strid(new Thread(barrier, duration(0.03))));
	for (size_t i=0; i<pool.size(); ++i)
	{
		lisle::Exit exit = pool[i].join();
		EXPECT_EQ(exit, lisle::terminated);
		Thread* thread = dynamic_cast<Thread*>((strand*)pool[i]);
		EXPECT_EQ(thread->hit, 3);
		EXPECT_EQ(thread->pass, 3);
	}
}
