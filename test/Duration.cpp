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
#include <gtest/gtest.h>
#include <lisle/Duration>

using namespace lisle;

TEST (DurationTest, ctorDefault)
{
	// Ensure duration is initialized to zero
	Duration d;
	EXPECT_EQ(d.sec(), 0);
	EXPECT_EQ(d.nsec(), 0);
}

TEST (DurationTest, ctorDouble)
{
	// Ensure duration is initialized from double
	{
		Duration d(3.1416);
		EXPECT_EQ(d.sec(), 3);
		EXPECT_LE(d.nsec(), 141600000u);
		EXPECT_GE(d.nsec(), 141599999u);
	}
	{
		Duration d(-3.1416);
		EXPECT_EQ(d.sec(), -3);
		EXPECT_LE(d.nsec(), 141600000u);
		EXPECT_GE(d.nsec(), 141599999u);
	}
}

TEST (DurationTest, ctorCopy)
{
	// Ensure duration is copied
	Duration i(3.1416);
	Duration d(i);
	EXPECT_EQ(d.sec(), 3);
	EXPECT_LE(d.nsec(), 141600000u);
	EXPECT_GE(d.nsec(), 141599999u);
}

TEST (DurationTest, operAssign)
{
	// Ensure duration is assigned
	Duration i(3.1416);
	Duration d;
	d = i;
	EXPECT_EQ(d.sec(), 3);
	EXPECT_LE(d.nsec(), 141600000u);
	EXPECT_GE(d.nsec(), 141599999u);
}

TEST (DurationTest, negator)
{
	// Ensure the unitary '-' operator is doing its job
	{
		Duration i(3.1416);
		Duration d = -i;
		EXPECT_EQ(d.sec(), -3);
		EXPECT_LE(d.nsec(), 141600000u);
		EXPECT_GE(d.nsec(), 141599999u);
	}
	{
		Duration i(-3.1416);
		Duration d = -i;
		EXPECT_EQ(d.sec(), 3);
		EXPECT_LE(d.nsec(), 141600000u);
		EXPECT_GE(d.nsec(), 141599999u);
	}
}

TEST (DurationTest, operEquals)
{
	// Ensure operator == works
	Duration d1(3.1416);
	Duration d2(3.1416);
	Duration d3(2.1841);
	EXPECT_TRUE(d1 == d2);
	EXPECT_FALSE(d1 == d3);
}

TEST (DurationTest, operDiffers)
{
	// Ensure operator != works
	Duration d1(3.1416);
	Duration d2(3.1416);
	Duration d3(2.1841);
	EXPECT_FALSE(d1 != d2);
	EXPECT_TRUE(d1 != d3);
}

TEST (DurationTest, operLess)
{
	// Ensure operator < works
	Duration d1(3.1416);
	Duration d2(3.1416);
	Duration d3(2.1841);
	EXPECT_FALSE(d1 < d2);
	EXPECT_TRUE(d3 < d1);
}

TEST (DurationTest, operMore)
{
	// Ensure operator > works
	Duration d1(3.1416);
	Duration d2(3.1416);
	Duration d3(2.1841);
	EXPECT_FALSE(d1 > d2);
	EXPECT_TRUE(d1 > d3);
}

TEST (DurationTest, operLessOrEqual)
{
	// Ensure operator <= works
	Duration d1(3.1416);
	Duration d2(3.1416);
	Duration d3(2.1841);
	EXPECT_TRUE(d1 <= d2);
	EXPECT_TRUE(d3 <= d1);
}

TEST (DurationTest, operMoreOrEqual)
{
	// Ensure operator >= works
	Duration d1(3.1416);
	Duration d2(3.1416);
	Duration d3(2.1841);
	EXPECT_TRUE(d1 >= d2);
	EXPECT_TRUE(d1 >= d3);
}

TEST (DurationTest, operAddAssign)
{
	// Ensure operator += works
	Duration d1(3.1416);
	Duration d2(2.1841);
	d1 += d2;
	EXPECT_EQ(d1.sec(), 5);
	EXPECT_LE(d1.nsec(), 325700000u);
	EXPECT_GE(d1.nsec(), 325699998u);
}

TEST (DurationTest, operSubAssign)
{
	// Ensure operator -= works
	Duration d1(3.1416);
	Duration d2(2.1841);
	d1 -= d2;
	EXPECT_EQ(d1.sec(), 0);
	EXPECT_LE(d1.nsec(), 957500000u);
	EXPECT_GE(d1.nsec(), 957499999u);
}

TEST (DurationTest, operAdd)
{
	// Ensure operator + works
	Duration d1(3.1416);
	Duration d2(2.1841);
	Duration d = d1+d2;
	EXPECT_EQ(d.sec(), 5);
	EXPECT_LE(d.nsec(), 325700000u);
	EXPECT_GE(d.nsec(), 325699998u);
}

TEST (DurationTest, operSub)
{
	// Ensure operator - works
	Duration d1(3.1416);
	Duration d2(2.1841);
	Duration d = d1-d2;
	EXPECT_EQ(d.sec(), 0);
	EXPECT_LE(d.nsec(), 957500000u);
	EXPECT_GE(d.nsec(), 957499999u);
}
