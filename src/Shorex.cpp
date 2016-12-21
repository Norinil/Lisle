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
#include <lisle/Shorex>
#include <lisle/exceptions>
#include <lisle/assert>
#include <lisle/Acquirer>
#include <lisle/Releaser>
#include <lisle/prioqueue>
#include <lisle/list>
#include <lisle/self>
#include <new>

#ifdef _MSC_VER
#pragma warning (disable : 4290 4715 4458 4459)
#endif

namespace lisle {

Shorex::~Shorex ()
throw (permission)
{}

Shorex::Shorex (Access priority)
throw (resource)
: soe(priority)
{}

bool Shorex::trylock (Access access)
throw ()
{
	switch (access)
	{
		case shared : return soe.trylockshared();
		case exclusive : return soe.trylockexclusive();
	}
}

void Shorex::lock (Access access)
throw (deadlock)
{
	switch (access)
	{
		case shared : soe.lockshared(); break;
		case exclusive : soe.lockexclusive(); break;
	}
}

void Shorex::unlock ()
throw (permission)
{
	soe.unlock();
}

Shorex::data::~data ()
{
	assert((this->exclusive == 0) && (this->shared.count == 0), permission());
}

bool Shorex::data::trylockshared ()
{
	Acquirer device(this->guard);
	if (this->exclusive != NULL)
		return false;
	else
	{
		threadle self(lisle::self);
		list owners(&this->shared.owner);
		if (!owners.member(self))
		{
			this->shared.count++;
			owners.append(self);
		}
		return true;
	}
}

bool Shorex::data::trylockexclusive ()
{
	Acquirer device(this->guard);
	if (this->shared.count != 0)
		return false;
	else
	{
		if (this->exclusive != NULL)
			return false;
		else
		{
			threadle self(lisle::self);
			this->exclusive = self;
			return true;
		}
	}
}

bool Shorex::data::canlockshared ()
{
	/* Checks if we can acquire a shared lock.
	 * Warning: this->guard *must* be locked before calling this function.
	 */
	
	// Already exclusive locked: can't lock.
	if (this->exclusive != NULL)
		return false;
	
	// Priority on shared locks: can lock.
	if (this->priority == lisle::shared)
		return true;

	// Priority on exclusive lock, but none are waiting: can lock.
	if (this->waiting.exclusive == NULL)
		return true;

	// Exclusive requests are waiting, but this is already shared: can lock.
	if (this->shared.count != 0)
		return true;

	// Exclusive requests are waiting and this is a new lock: can't lock.
	return false;
}

void Shorex::data::lockshared ()
{
	/* 1) Check deadlock:
	 *    If self thread already member of lock owners we have a deadlock.
	 * 2) Check if we must wait:
	 *    If we can't shared lock we must wait.
	 * 3) Perform shared lock:
	 *    Increment shared count and add self thread in shared owner list.
	 */
	
	Acquirer device(this->guard);
	threadle self(lisle::self);
	list owners(&this->shared.owner);
	bool wait;
	if (this->shared.count != 0)
		assert(!owners.member(self), lisle::deadlock());
	wait = true;
	while (wait)
	{
		if (this->canlockshared())
		{
			wait = false;
			this->shared.count++;
			owners.append(self);
		}
		if (wait)
		{
			prioqueue waitqueue(&this->waiting.shared);
			waitqueue.push(self);
			{
				Releaser release(this->guard);
				Acquirer thread(self->guard);
				self->waitrestart();
			}
		}
	}
}

void Shorex::data::lockexclusive ()
{
	/* 1) Check deadlock:
	 *    If self thread is owner of exclusive lock we have a deadlock.
	 * 2) Check if we must wait:
	 *    If some other thread(s) hold a shared lock we must wait.
	 *    If another thread holds an exclusive lock we must wait.
	 * 3) Perform exclusive lock:
	 *    Set exclusive to self.
	 * We must execute 2) and 3) in a loop. We must recheck if we can realy acquire
	 * the exclusive lock each time somebody restarts us.
	 */
	
	Acquirer device(this->guard);
	threadle self(lisle::self);
	bool wait;
	if (this->exclusive != NULL)
		assert(!this->exclusive->equal(self), lisle::deadlock());
	wait = true;
	while (wait)
	{
		if ((this->shared.count == 0) && (this->exclusive == NULL))
		{
			wait = false;
			this->exclusive = self;   // Acquire exclusive lock
		}
		if (wait)
		{
			prioqueue waitqueue(&this->waiting.exclusive);
			waitqueue.push(self);
			{
				Releaser release(this->guard);
				Acquirer thread(self->guard);
				self->waitrestart();
			}
		}
	}
}

void Shorex::data::unlock ()
{
	threadle self(lisle::self);
	thread* waiting = NULL;
	{
		Acquirer device(this->guard);
		bool restart = false;
		if (this->exclusive == NULL)
		{
			// Release shared lock
			list owners(&this->shared.owner);
			assert(this->shared.count != 0, lisle::permission());
			assert(owners.remove(self), lisle::permission());
			this->shared.count--;
			if (this->shared.count == 0)
				restart = true;
		}
		else
		{
			// Release exclusive lock
			assert(this->exclusive->equal(self), lisle::permission());
			this->exclusive = NULL;
			restart = true;
		}
		if (restart)
		{
			switch (this->priority)
			{
				case lisle::exclusive :
					if (this->waiting.exclusive != NULL)
					{
						prioqueue waitqueue(&this->waiting.exclusive);
						waiting = waitqueue.top();
						waitqueue.pop();
					}
					else if (this->waiting.shared != NULL)
					{
						waiting = this->waiting.shared;
						this->waiting.shared = NULL;
					}
				break;
				case lisle::shared :
					if (this->waiting.shared != NULL)
					{
						waiting = this->waiting.shared;
						this->waiting.shared = NULL;
					}
					else if (this->waiting.exclusive != NULL)
					{
						prioqueue waitqueue(&this->waiting.exclusive);
						waiting = waitqueue.top();
						waitqueue.pop();
					}
				break;
			}
		}
	}
	if (waiting != NULL)
	{
		prioqueue waitqueue(&waiting);
		while (!waitqueue.empty())
		{
			waitqueue.top()->restart();
			waitqueue.pop();
		}
		self->yield();   // Give restarted threads a chance to run
	}
}

}
