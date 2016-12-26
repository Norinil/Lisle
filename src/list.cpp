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
#include "list.h"

namespace lisle {

bool list::member (intern::thread* item)
{
	intern::thread* cur;
	if (*head == 0)
		return false;
	cur = *head;
	while (cur != 0)
	{
		if (cur->equal(*item))
			return true;
		else
			cur = cur->next.list;
	}
	return false;
}

void list::append (intern::thread* item)
{
	intern::thread* cur;
	if (*head == 0)
		*head = item;
	else
	{
		cur = *head;
		while (cur->next.list != 0)
			cur = cur->next.list;
		cur->next.list = item;
	}
}

bool list::remove (intern::thread* item)
{
	intern::thread* cur;
	intern::thread* prev;
	if (*head == 0)
		return false;
	cur = *head;
	if (cur->equal(*item))
	{
		*head = cur->next.list;
		cur->next.list = 0;
		return true;
	}
	prev = cur;
	cur = cur->next.list;
	while (cur != 0)
	{
		if (cur->equal(*item))
		{
			prev->next.list = cur->next.list;
			cur->next.list = 0;
			return true;
		}
		prev = cur;
		cur = cur->next.list;
	}
	return false;
}

}
