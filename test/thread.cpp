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
#include <lisle/thread.h>

using namespace lisle;

static void worker00 () {}
static void worker01 (handle<double> arg) {}
static handle<double> worker10 () { return handle<double>(new double(2.1218)); }
static handle<double> worker11 (handle<double> arg) { return handle<double>(new double(2.1218)); }

TEST (Thread, ctor)
{
	thread start;
	EXPECT_NU(start.main());
	EXPECT_EQ(thread::joinable, start.state());
	EXPECT_EQ(thread::running, start.mode());
}

TEST (Thread, ctor00)
{
	thread start(worker00);
	EXPECT_EQ(&worker00, start.main());
	EXPECT_EQ(thread::joinable, start.state());
	EXPECT_EQ(thread::running, start.mode());
}

TEST (Thread, ctor01)
{
	thread start(worker01, handle<double>(new double(3.1416)));
	EXPECT_EQ((void(*)())&worker01, start.main());
	EXPECT_EQ(thread::joinable, start.state());
	EXPECT_EQ(thread::running, start.mode());
}

TEST (Thread, ctor10)
{
	thread start(worker10);
	EXPECT_EQ((void(*)())&worker10, start.main());
	EXPECT_EQ(thread::joinable, start.state());
	EXPECT_EQ(thread::running, start.mode());
}

TEST (Thread, ctor11)
{
	thread start(worker11, handle<double>(new double(3.1416)));
	EXPECT_EQ((void(*)())&worker11, start.main());
	EXPECT_EQ(thread::joinable, start.state());
	EXPECT_EQ(thread::running, start.mode());
}

TEST (Thread, setmain00)
{
	thread start;
	start.set(worker00);
	EXPECT_EQ(&worker00, start.main());
	EXPECT_EQ(thread::joinable, start.state());
	EXPECT_EQ(thread::running, start.mode());
}

TEST (Thread, setmain01)
{
	thread start;
	start.set(worker01, handle<double>(new double(3.1416)));
	EXPECT_EQ((void(*)())&worker01, start.main());
	EXPECT_EQ(thread::joinable, start.state());
	EXPECT_EQ(thread::running, start.mode());
}

TEST (Thread, setmain10)
{
	thread start;
	start.set(worker10);
	EXPECT_EQ((void(*)())&worker10, start.main());
	EXPECT_EQ(thread::joinable, start.state());
	EXPECT_EQ(thread::running, start.mode());
}

TEST (Thread, setmain11)
{
	thread start;
	start.set(worker11, handle<double>(new double(3.1416)));
	EXPECT_EQ((void(*)())&worker11, start.main());
	EXPECT_EQ(thread::joinable, start.state());
	EXPECT_EQ(thread::running, start.mode());
}
