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
#include <lisle/strand.h>
#include <lisle/strid.h>

using namespace lisle;

#ifdef _MSC_VER
#pragma warning (disable:4512)
#endif

TEST (Strand, success)
{
	bool done = false;
	class Thread : public strand
	{
	public:
		Thread (bool& done) : base(), done(done) {}
		void main () { done = true; }
	private:
		typedef strand base;
		bool& done;
	};
	ASSERT_FALSE(done);
	strid thr(new Thread(done));
	thr.join();
	ASSERT_TRUE(done);
}
