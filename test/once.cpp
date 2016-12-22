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
#include <lisle/once.h>
#include <lisle/create.h>
#include <lisle/self.h>

using namespace lisle;

static int one = 0;

static void routine ()
{
	one++;
}

once init(routine);

static void worker ()
{
	yield(); // give other threads a chance to run
	init.run();
}

static void workers ()
{
	yield(); // give other threads a chance to run
	thrid tid[2];
	tid[0] = create(thread(worker));
	tid[1] = create(thread(worker));
	tid[0].join();
	tid[1].join();
}

TEST (Once, success)
{
	// Ensure a once control is run only once, whatever thread actually runs it
	thrid tid[2];
	tid[0] = create(thread(workers));
	tid[1] = create(thread(worker));
	tid[0].join();
	tid[1].join();
	EXPECT_EQ(1, one);
}
