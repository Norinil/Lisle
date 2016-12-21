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
#include "../src/prioqueue.h"

using namespace lisle;

/// Test that prioqueue pushes threads with respect to their priority
TEST (prioqueueTest, priority)
{
	threadle thr[3];
	thr[0] = new thread;
	thr[1] = new thread;
	thr[2] = new thread;
	thr[0]->priority = 1;
	thr[1]->priority = 2;
	thr[2]->priority = 0;
	
	thread* waiting = 0;
	prioqueue queue(&waiting);
	queue.push(thr[0]);
	queue.push(thr[1]);
	queue.push(thr[2]);
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[1]);
	ASSERT_NO_THROW(queue.pop());

	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[0]);
	ASSERT_NO_THROW(queue.pop());

	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[2]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_TRUE(queue.empty());
}

/// Test that prioqueue pushes threads with respect to insertion order
TEST (prioqueueTest, order)
{
	threadle thr[3];
	thr[0] = new thread;
	thr[1] = new thread;
	thr[2] = new thread;
	thr[0]->priority = 0;
	thr[1]->priority = 0;
	thr[2]->priority = 0;
	
	thread* waiting = 0;
	prioqueue queue(&waiting);
	queue.push(thr[2]);
	queue.push(thr[0]);
	queue.push(thr[1]);

	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[2]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[0]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[1]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_TRUE(queue.empty());
}

/// Test that prioqueue pushes threads with respect their priority and insertion order
TEST (prioqueueTest, mixed)
{
	threadle thr[9];
	thr[0] = new thread;
	thr[1] = new thread;
	thr[2] = new thread;
	thr[3] = new thread;
	thr[4] = new thread;
	thr[5] = new thread;
	thr[6] = new thread;
	thr[7] = new thread;
	thr[8] = new thread;
	thr[0]->priority = 0;
	thr[1]->priority = 0;
	thr[2]->priority = 0;
	thr[3]->priority = 1;
	thr[4]->priority = 1;
	thr[5]->priority = 1;
	thr[6]->priority = 2;
	thr[7]->priority = 2;
	thr[8]->priority = 2;
	
	thread* waiting = 0;
	prioqueue queue(&waiting);
	queue.push(thr[4]);
	queue.push(thr[5]);
	queue.push(thr[2]);
	queue.push(thr[8]);
	queue.push(thr[6]);
	queue.push(thr[0]);
	queue.push(thr[1]);
	queue.push(thr[7]);
	queue.push(thr[3]);
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[8]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[6]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[7]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[4]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[5]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[3]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[2]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[0]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_FALSE(queue.empty());
	EXPECT_EQ(queue.top(), thr[1]);
	ASSERT_NO_THROW(queue.pop());
	
	ASSERT_TRUE(queue.empty());
}
