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
*/
#pragma once
#include <lisle/thread.h>
#include <lisle/handle.h>
#include <lisle/exception.h>
#include <lisle/self.h>
#include <lisle/intern/threadle.h>
#include <cstdint>

#ifdef _MSC_VER
#pragma warning (push,2)
#endif

namespace lisle { class thrid; }
namespace lisle { thrid create (const thread&, const schedule&) throw (resource, permission, virthread); }

namespace lisle {
enum Exit { terminated, canceled, exceptioned };
class thrid
{
public:
	~thrid () {}
	thrid ();
	thrid (const thrid& that) : thr(that.thr) {}
	thrid& operator = (const thrid& that) { this->thr = that.thr; return *this; }
	operator uint32_t () const { return thr->id(); }
	bool operator == (const thrid& that) const { return this->thr->equal(that.thr); }
	bool operator != (const thrid& that) const { return !this->thr->equal(that.thr); }
	void detach () throw (lisle::detach);
	void cancel () throw ();
	void resume () throw ();
	Exit join () throw (lisle::join, lisle::deadlock, lisle::thrcancel)
	{
		return ijoin();
	}
	template <typename TRT> Exit join (handle<TRT>& hretd) throw (lisle::join, lisle::deadlock, lisle::thrcancel)
	{
		Exit result(ijoin());
		if (thr->hretd.handle<TRT>() != 0)
			hretd = *thr->hretd.handle<TRT>();
		return result;
	}
private:
	intern::threadle thr;
	thrid (const intern::threadle& thr); // Used by Thread::create()
	Exit ijoin () throw (lisle::join, lisle::deadlock, lisle::thrcancel);
	friend thrid lisle::create (const thread&, const schedule&) throw (resource, permission, virthread);
};
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
