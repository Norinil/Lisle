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
// Lisle internal self.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <lisle/sys/local.h>
#include <lisle/intern/threadle.h>

namespace lisle {
namespace intern {
class Self
{
public:
	~Self ();
	Self ();
	operator threadle () const;
	void create (const threadle& handle);
	void destroy ();
private:
	sys::local key;
	Self (const Self&);
	Self& operator = (const Self&);
};
extern Self self;
}
}
