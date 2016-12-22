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
#include <lisle/duration.h>

using namespace lisle;

TEST (Duration, ctor)
{
	// Ensure duration is initialized to zero
	duration d;
	EXPECT_EQ(d.sec(), 0);
	EXPECT_EQ(d.nsec(), 0);
}

TEST (Duration, ctor_double)
{
	// Ensure duration is initialized from double
	{
		duration d(3.1416);
		EXPECT_EQ(d.sec(), 3);
		EXPECT_LE(d.nsec(), 141600000u);
		EXPECT_GE(d.nsec(), 141599999u);
	}
	{
		duration d(-3.1416);
		EXPECT_EQ(d.sec(), -3);
		EXPECT_LE(d.nsec(), 141600000u);
		EXPECT_GE(d.nsec(), 141599999u);
	}
}

TEST (Duration, ctor_copy)
{
	// Ensure duration is copied
	duration i(3.1416);
	duration d(i);
	EXPECT_EQ(d.sec(), 3);
	EXPECT_LE(d.nsec(), 141600000u);
	EXPECT_GE(d.nsec(), 141599999u);
}

TEST (Duration, oper_assign)
{
	// Ensure duration is assigned
	duration i(3.1416);
	duration d;
	d = i;
	EXPECT_EQ(d.sec(), 3);
	EXPECT_LE(d.nsec(), 141600000u);
	EXPECT_GE(d.nsec(), 141599999u);
}

TEST (Duration, negator)
{
	// Ensure the unitary '-' operator is doing its job
	{
		duration i(3.1416);
		duration d = -i;
		EXPECT_EQ(d.sec(), -3);
		EXPECT_LE(d.nsec(), 141600000u);
		EXPECT_GE(d.nsec(), 141599999u);
	}
	{
		duration i(-3.1416);
		duration d = -i;
		EXPECT_EQ(d.sec(), 3);
		EXPECT_LE(d.nsec(), 141600000u);
		EXPECT_GE(d.nsec(), 141599999u);
	}
}

TEST (Duration, oper_equals)
{
	// Ensure operator == works
	duration d1(3.1416);
	duration d2(3.1416);
	duration d3(2.1841);
	EXPECT_TRUE(d1 == d2);
	EXPECT_FALSE(d1 == d3);
}

TEST (Duration, oper_differs)
{
	// Ensure operator != works
	duration d1(3.1416);
	duration d2(3.1416);
	duration d3(2.1841);
	EXPECT_FALSE(d1 != d2);
	EXPECT_TRUE(d1 != d3);
}

TEST (Duration, oper_less)
{
	// Ensure operator < works
	duration d1(3.1416);
	duration d2(3.1416);
	duration d3(2.1841);
	EXPECT_FALSE(d1 < d2);
	EXPECT_TRUE(d3 < d1);
}

TEST (Duration, oper_more)
{
	// Ensure operator > works
	duration d1(3.1416);
	duration d2(3.1416);
	duration d3(2.1841);
	EXPECT_FALSE(d1 > d2);
	EXPECT_TRUE(d1 > d3);
}

TEST (Duration, oper_less_or_equal)
{
	// Ensure operator <= works
	duration d1(3.1416);
	duration d2(3.1416);
	duration d3(2.1841);
	EXPECT_TRUE(d1 <= d2);
	EXPECT_TRUE(d3 <= d1);
}

TEST (Duration, oper_more_or_equal)
{
	// Ensure operator >= works
	duration d1(3.1416);
	duration d2(3.1416);
	duration d3(2.1841);
	EXPECT_TRUE(d1 >= d2);
	EXPECT_TRUE(d1 >= d3);
}

TEST (Duration, oper_add_assign)
{
	// Ensure operator += works
	duration d1(3.1416);
	duration d2(2.1841);
	d1 += d2;
	EXPECT_EQ(d1.sec(), 5);
	EXPECT_LE(d1.nsec(), 325700000u);
	EXPECT_GE(d1.nsec(), 325699998u);
}

TEST (Duration, oper_sub_assign)
{
	// Ensure operator -= works
	duration d1(3.1416);
	duration d2(2.1841);
	d1 -= d2;
	EXPECT_EQ(d1.sec(), 0);
	EXPECT_LE(d1.nsec(), 957500000u);
	EXPECT_GE(d1.nsec(), 957499999u);
}

TEST (Duration, oper_add)
{
	// Ensure operator + works
	duration d1(3.1416);
	duration d2(2.1841);
	duration d = d1+d2;
	EXPECT_EQ(d.sec(), 5);
	EXPECT_LE(d.nsec(), 325700000u);
	EXPECT_GE(d.nsec(), 325699998u);
}

TEST (Duration, oper_sub)
{
	// Ensure operator - works
	duration d1(3.1416);
	duration d2(2.1841);
	duration d = d1-d2;
	EXPECT_EQ(d.sec(), 0);
	EXPECT_LE(d.nsec(), 957500000u);
	EXPECT_GE(d.nsec(), 957499999u);
}
