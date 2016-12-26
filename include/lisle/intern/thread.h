/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017, Markus Schütz
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
// Lisle internal thread.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/sys/thread.h>
#include <lisle/duration.h>

namespace lisle {
namespace intern {
struct thread : public sys::thread
{
	enum state { initial, suspended, running, canceling, terminal, exceptioned, canceled, terminated, joined } state; // thread state
	struct detach
	{
		enum state { joinable, detached } state; // thread detach state
		detach () : state(joinable) {}
	} detach;
	struct cancel
	{
		enum state { disable, enable } state;
		struct waiting
		{
			thread* queue;
			mutex* guard;
			waiting () : queue(0), guard(0) {}
		} waiting;
		bool pending;
		thread* joining;
		cancel () : state(enable), pending(false), joining(0) {}
	} cancel;
	struct next
	{
		thread* queue; // link to next thread in queue (waiting)
		thread* list; // link to next thread in list (owners)
		next () : queue(0), list(0) {}
	} next;
	thread* joiner;
	anondle hretd;
	int priority;
	thread () : state(initial), joiner(0) {}
	void suspend ();
	void resume ();
	void waitrestart ();
	void waitrestartcancel (thread* waiting, mutex& device);
	void waitrestartcancel (const duration& span, thread* waiting, mutex& device);
	void restart ();
	void testcancel ();
	void waitjoiner ();
	void waitjoiningcancel (thread* thread);
private:
	typedef sys::thread base;
	void waitrestart (const duration& span);
};
}
}
