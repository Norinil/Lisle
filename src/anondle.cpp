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
#include <lisle/anondle.h>

namespace lisle {

anondle::~anondle ()
{
	if (data.handle)
		(*data.unlink)(data.handle);
}

anondle::anondle ()
{}

anondle::anondle (const anondle& that)
: data(0, that.data.link, that.data.unlink)
{
	if (that.data.handle)
		this->data.handle = (*data.link)(that.data.handle);
}

anondle& anondle::operator = (const anondle& that)
{
	// if this is linked then unlink it
	if (data.handle)
		(*data.unlink)(data.handle);
	// link this to that
	this->data.link = that.data.link;
	this->data.unlink = that.data.unlink;
	if (that.data.handle)
		this->data.handle = (*data.link)(that.data.handle);
	else
		this->data.handle = 0;
	return *this;
}
	
}
