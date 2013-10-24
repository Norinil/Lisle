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
//
// Test for thread suspend and resume.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include <gtest/gtest.h>
#include <lisle/Thread.Id>
#include <lisle/create>
#include <lisle/self>
#include <cstdio>

using namespace lisle;

static void thread1 ()
{
	Thread::Id self;
	set(Thread::Cancel::Enable);
	for (;;)
	{
		printf(".%x.\n", (uint32_t)self);
		fflush(stdout);
		sleep(Duration(1));
		testcancel();
	}
}

static void thread2 ()
{
	Thread::Id self;
	for (;;)
	{
		printf(".%x.\n", (uint32_t)self);
		fflush(stdout);
		sleep(Duration(1));
		testcancel();
	}
}

static void master ()
{
  Thread::Id tid[2];

	printf("3 \n");
  fflush(stdout);
  tid[0] = create(Thread::Start(&thread1));
	tid[1] = create(Thread::Start(&thread2, Thread::Start::Joinable, Thread::Start::Suspended));
	sleep(Duration(3));
  tid[0].resume();
	tid[0].resume(); // multiple resume shouldn't fail, only be ignored when thread already resumed
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
  tid[0].suspend();
  tid[0].suspend(); // multiple suspend shouldn't fail, only be ignored when thread already suspended
  printf("\nSleeping 3 secs with '.' printing thread suspended...\n");
  fflush(stdout);
	sleep(Duration(3));

  printf("\n7 \n");
  fflush(stdout);
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
	sleep(Duration(7));
  tid[1].suspend();
  printf("\nSleeping 3 secs with '.' printing thread suspended...\n");
  fflush(stdout);
	sleep(Duration(7));

  printf("\n3 \n");
  fflush(stdout);
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
	sleep(Duration(3));
  tid[0].suspend();
  printf("\nSleeping 3 secs with '.' printing thread suspended...\n");
  fflush(stdout);
	sleep(Duration(3));

  printf("\n7 \n");
  fflush(stdout);
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
  tid[1].resume();
	sleep(Duration(7));
  tid[1].suspend();
  printf("\nSleeping 3 secs with '.' printing thread suspended...\n");
  fflush(stdout);
	sleep(Duration(3));

  printf("\n3 \n");
  fflush(stdout);
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
  tid[0].resume();
	sleep(Duration(3));

  tid[0].resume();
  tid[0].cancel();
  tid[0].join();
  tid[1].resume();
  tid[1].cancel();
  tid[1].join();
  printf("\n");
}

TEST (sureTest, success)
{
	ASSERT_NO_THROW(master());
}
