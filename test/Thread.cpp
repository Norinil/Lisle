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
#include <lisle/Thread>

using namespace lisle;

static void worker00 () {}
static void worker01 (Handle<double> arg) {}
static Handle<double> worker10 () { return Handle<double>(new double(2.1218)); }
static Handle<double> worker11 (Handle<double> arg) { return Handle<double>(new double(2.1218)); }

TEST (ThreadTest, ctorDefault)
{
	Thread start;
	EXPECT_NU(start.main());
	EXPECT_EQ(Thread::Joinable, start.state());
	EXPECT_EQ(Thread::Running, start.mode());
}

TEST (ThreadTest, ctor00)
{
	Thread start(worker00);
	EXPECT_EQ(&worker00, start.main());
	EXPECT_EQ(Thread::Joinable, start.state());
	EXPECT_EQ(Thread::Running, start.mode());
}

TEST (ThreadTest, ctor01)
{
	Thread start(worker01, Handle<double>(new double(3.1416)));
	EXPECT_EQ((void(*)())&worker01, start.main());
	EXPECT_EQ(Thread::Joinable, start.state());
	EXPECT_EQ(Thread::Running, start.mode());
}

TEST (ThreadTest, ctor10)
{
	Thread start(worker10);
	EXPECT_EQ((void(*)())&worker10, start.main());
	EXPECT_EQ(Thread::Joinable, start.state());
	EXPECT_EQ(Thread::Running, start.mode());
}

TEST (ThreadTest, ctor11)
{
	Thread start(worker11, Handle<double>(new double(3.1416)));
	EXPECT_EQ((void(*)())&worker11, start.main());
	EXPECT_EQ(Thread::Joinable, start.state());
	EXPECT_EQ(Thread::Running, start.mode());
}

TEST (ThreadTest, setmain00)
{
	Thread start;
	start.set(worker00);
	EXPECT_EQ(&worker00, start.main());
	EXPECT_EQ(Thread::Joinable, start.state());
	EXPECT_EQ(Thread::Running, start.mode());
}

TEST (ThreadTest, setmain01)
{
	Thread start;
	start.set(worker01, Handle<double>(new double(3.1416)));
	EXPECT_EQ((void(*)())&worker01, start.main());
	EXPECT_EQ(Thread::Joinable, start.state());
	EXPECT_EQ(Thread::Running, start.mode());
}

TEST (ThreadTest, setmain10)
{
	Thread start;
	start.set(worker10);
	EXPECT_EQ((void(*)())&worker10, start.main());
	EXPECT_EQ(Thread::Joinable, start.state());
	EXPECT_EQ(Thread::Running, start.mode());
}

TEST (ThreadTest, setmain11)
{
	Thread start;
	start.set(worker11, Handle<double>(new double(3.1416)));
	EXPECT_EQ((void(*)())&worker11, start.main());
	EXPECT_EQ(Thread::Joinable, start.state());
	EXPECT_EQ(Thread::Running, start.mode());
}
