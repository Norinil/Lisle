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
#include <lisle/Once>
#include <lisle/create>
#include <lisle/self>

using namespace lisle;

static int one = 0;

static void once ()
{
	one++;
}

Once init(once);

static void worker ()
{
	yield(); // give other threads a chance to run
	init.run();
}

static void workers ()
{
	yield(); // give other threads a chance to run
	Thrid tid[2];
	tid[0] = create(Thread(worker));
	tid[1] = create(Thread(worker));
	tid[0].join();
	tid[1].join();
}

TEST (OnceTest, success)
{
	// Ensure a once control is run only once, whatever thread actually runs it
	Thrid tid[2];
	tid[0] = create(Thread(workers));
	tid[1] = create(Thread(worker));
	tid[0].join();
	tid[1].join();
	EXPECT_EQ(1, one);
}
