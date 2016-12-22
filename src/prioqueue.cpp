/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005-2012, Markus Schütz
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
#include "prioqueue.h"

namespace lisle {

void prioqueue::push (thread* item)
{
	if (this->empty())
	{
		*queue = item;
		return;
	}

	// priorities are ordered numerically: 67 is higher priority than 63
	// the higher the priority, the closer it must be inserted to the queue top
	// insert items with same priorities in insertion order
	thread* curr = *queue;
	thread* prev = 0;
	while ((item->priority <= curr->priority) && curr->next.queue)
	{
		prev = curr;
		curr = curr->next.queue;
	}
	if (item->priority > curr->priority)
	{ // insert before curr
		item->next.queue = curr;
		if (prev)
			prev->next.queue = item;
		else
			*queue = item;
	}
	else
	{ // insert after curr
		item->next.queue = curr->next.queue;
		curr->next.queue = item;
	}
}

void prioqueue::pop ()
{
	thread* cur = *queue;
	if (*queue != NULL)
	{
		*queue = (*queue)->next.queue;
		cur->next.queue = NULL;
	}
}

bool prioqueue::remove (thread* item)
{
	thread* cur;
	thread* prev;
	if (*queue == NULL)
		return false;
	cur = *queue;
	if (cur->equal(*item))
	{
		*queue = cur->next.queue;
		cur->next.queue = NULL;
		return true;
	}
	prev = cur;
	cur = cur->next.queue;
	while (cur != NULL)
	{
		if (cur->equal(*item))
		{
			prev->next.queue = cur->next.queue;
			cur->next.queue = NULL;
			return true;
		}
		prev = cur;
		cur = cur->next.queue;
	}
	return false;
}

}
