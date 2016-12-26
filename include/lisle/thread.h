/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2012, Markus Schütz
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
// Thread start descriptor.
// Stores the thread's main function and the arguments handle.
// Everything is stored as void* (untyped pointer) so that:
//   1) Thread::Start can be untemplated itself.
//   2) Thread startup code can be linked into the library.
// Reason 1) is to keep things simple for the end developer.
// Reason 2) is to have as few as possible things in the _lisle_ namespace
// visible in installed interfaces. End developers should not see it as far
// as possible so they can concentrate on their work, not on library internal
// details.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/exception.h>
#include <lisle/handle.h>
#include <lisle/anondle.h>
#include <lisle/schedule.h>

namespace lisle { class thread; }
namespace lisle { void launch (const lisle::thread&); }

namespace lisle {
class thread
{
	friend void lisle::launch (const lisle::thread&);
public:
	typedef anondle(* Caller)(void(*)(),anondle&);
	typedef void(* Callee)();
	enum State { joinable, detached };
	enum Mode { running, suspended };
	~thread () {}
	thread () {}
	thread (const thread& that) : data(that.data) {}
	thread& operator = (const thread& that) { this->data = that.data; return *this; }
	thread (void(* main)(), Mode mode = running, State state = joinable) : data(&caller00, main, mode, state) {}
	template <typename TAT> thread (void(* main)(handle<TAT>), const handle<TAT>& args, Mode mode = running, State state = joinable) : data((Caller)(anondle(*)(void(*)(handle<TAT>),anondle&))&caller01<TAT>, (Callee)main, args, mode, state) {}
	template <typename TRT> thread (handle<TRT>(* main)(), Mode mode = running, State state = joinable) : data((Caller)(anondle(*)(handle<TRT>(*)(),anondle&))&caller10, (Callee)main, mode, state) {}
	template <typename TRT, typename TAT> thread (handle<TRT>(* main)(handle<TAT>), const handle<TAT>& args, Mode mode = running, State state = joinable) : data((Caller)(anondle(*)(handle<TRT>(*)(handle<TAT>),anondle&))&caller11, (Callee)main, args, mode, state) {}
	void set (void(* main)()) { data.call = &caller00; data.main = main; data.args = anondle(); }
	template <typename TAT> void set (void(* main)(handle<TAT>), const handle<TAT>& args) { data.call = (Caller)(anondle(*)(void(*)(handle<TAT>),anondle&))&caller01; data.main = (Callee)main; data.args = args; }
	template <typename TRT> void set (handle<TRT>(* main)()) { data.call = (Caller)(anondle(*)(handle<TRT>(*)(),anondle&))&caller10; data.main = (Callee)main; data.args = anondle(); }
	template <typename TRT, typename TAT> void set (handle<TRT>(* main)(handle<TAT>), const handle<TAT>& args) { data.call = (Caller)(anondle(*)(handle<TRT>(*)(handle<TAT>),anondle&))&caller11; data.main = (Callee)main; data.args = args; }
	void set (State state) { data.state = state; }
	void set (Mode mode) { data.mode = mode; }
	Caller call () const { return data.call; }
	Callee main () const { return data.main; }
	anondle& args () const { return data.args; }
	State state () const { return data.state; }
	Mode mode () const { return data.mode; }
private:
	struct Data
	{
		Caller call;
		Callee main;
		mutable anondle args;
		Mode mode;
		State state;
		Data () : call(0), main(0), mode(running), state(joinable) {}
		Data (const Data& that) : call(that.call), main(that.main), args(that.args), mode(that.mode), state(that.state) {}
		Data (Caller call, void(* main)(), Mode mode, State state) : call(call), main(main), mode(mode), state(state) {}
		Data (Caller call, void(* main)(), const anondle& args, Mode mode, State state) : call(call), main(main), args(args), mode(mode), state(state) {}
	} data;
	static anondle caller00 (void(* main)(), anondle&)
	{
		(*main)();
		return anondle();
	}
	template <typename TAT> static anondle caller01 (void(* main)(handle<TAT>), anondle& args)
	{
		(*main)(*args.handle<TAT>());
		return anondle();
	}
	template <typename TRT> static anondle caller10 (handle<TRT>(* main)(), anondle&)
	{
		return anondle((*main)());
	}
	template <typename TRT, typename TAT> static anondle caller11 (handle<TRT>(* main)(handle<TAT>), anondle& args)
	{
		return anondle((*main)(*args.handle<TAT>()));
	}
};
}
