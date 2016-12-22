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
#pragma once
#include <lisle/anondle.h>
#include <exception>
#include <cstdint>

namespace lisle {

class resource : public std::exception
{
public:
	virtual const char* what () const throw () { return "resource"; }
};

class permission : public std::exception
{
public:
	virtual const char* what () const throw () { return "permission"; }
};

class deadlock : public std::exception
{
public:
	virtual const char* what () const throw () { return "deadlock"; }
};

class overflow : public std::exception
{
public:
	virtual const char* what () const throw () { return "overflow"; }
};

class timeout : public std::exception
{
public:
	virtual const char* what () const throw () { return "timeout"; }
};

class virthread : public std::exception
{ // virtual thread: a null function address was given to a thread creator
public:
	virtual const char* what () const throw () { return "virthread"; }
};

class join : public std::exception
{
public:
	enum reason { missing, detached, duplicate } reason;
	join (enum reason reason) : reason(reason) {}
	join (const join& that) : std::exception(that), reason(that.reason) {}
	virtual const char* what () const throw () { return "join"; }
};

class detach : public std::exception
{
public:
	enum reason { missing, detached } reason;
	detach (enum reason reason) : reason(reason) {}
	detach (const detach& that) : std::exception(that), reason(that.reason) {}
	virtual const char* what () const throw () { return "detach"; }
};

class threption // so one can catch (and rethrow) thrcancel and threxit in a single statement
{};

class thrcancel : public threption
{};

class threxit : public threption
{
public:
	anondle hretd; // handle to return data
	threxit (const anondle& hretd) : hretd(hretd) {}
	threxit (const threxit& that) : hretd(that.hretd) {}
};

}
