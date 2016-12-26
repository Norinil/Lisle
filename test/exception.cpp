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
#include <lisle/exception.h>

using namespace lisle;

TEST (Exception, forward_thrcancel_catching_threption)
{
	// Ensure we can forward thrcancel by catching threption
	bool canceled = false;
	try
	{
		try
		{
			throw thrcancel();
		}
		catch (threption&)
		{
			throw;
		}
		catch (...)
		{
			FAIL() << "to catch 'thrcancel' with 'threption'";
		}
	}
	catch (thrcancel&)
	{
		// ok, that's what lisle does, then flags the thread as canceled for join
		canceled = true;
	}
	catch (threption&)
	{
		FAIL() << "got 'threption', expected 'thrcancel'";
	}
	catch (...)
	{
		FAIL() << "got undefined exception, expected 'thrcancel'";
	}
	ASSERT_TRUE(canceled);
}

TEST (Exception, forward_threxit_catching_threption)
{
	// Ensure we can forward thrcancel by catching threption
	bool exited = false;
	try
	{
		try
		{
			throw threxit(anondle());
		}
		catch (threption&)
		{
			throw;
		}
		catch (...)
		{
			FAIL() << "to catch 'threxit' with 'threption'";
		}
	}
	catch (threxit&)
	{
		// ok, that's what lisle does, then flags the thread as exited for join
		exited = true;
	}
	catch (threption&)
	{
		FAIL() << "got 'threption', expected 'threxit'";
	}
	catch (...)
	{
		FAIL() << "got undefined exception, expected 'threxit'";
	}
	ASSERT_TRUE(exited);
}
