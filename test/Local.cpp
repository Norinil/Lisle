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
#include <lisle/Local>
#include <lisle/create>

using namespace lisle;

struct Data
{
	int x;
	int y;
	Data () : x(0), y(0) {}
	Data (int x, int y) : x(x), y(y) {}
};

static bool done = false; // shared data
static Local<Data> mine; // thread specific data

static void worker ()
{
	mine = Data(56, 78);
	done = true;
}

TEST (LocalTest, singleAssign)
{
	// Ensure thread specific data is thread specific
	done = false;
	ASSERT_NO_THROW(mine = Data(12, 34));
	EXPECT_EQ(12, mine->x);
	EXPECT_EQ(34, mine->y);
	Thrid tid = create(Thread(worker));
	tid.join();
	ASSERT_TRUE(done);
	EXPECT_EQ(12, mine->x);
	EXPECT_EQ(34, mine->y);
}

TEST (LocalTest, multipleAssign)
{
	// Ensure thread specific data is thread specific
	done = false;
	ASSERT_NO_THROW(mine = Data(9, 8));
	EXPECT_EQ(9, mine->x);
	EXPECT_EQ(8, mine->y);
	ASSERT_NO_THROW(mine = Data(12, 34));
	EXPECT_EQ(12, mine->x);
	EXPECT_EQ(34, mine->y);
	Thrid tid = create(Thread(worker));
	tid.join();
	ASSERT_TRUE(done);
	EXPECT_EQ(12, mine->x);
	EXPECT_EQ(34, mine->y);
}
