/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2003, Markus Schutz
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
// $Id: lisle.cpp 3516 2006-07-30 21:57:27Z schutzm $
//
// Project URL: http://lyric.sourceforge.net
//
// Description:
// Functions to get informations about the library.
// All functions are of type lisle_<something>.
// These functions were writen to be used in GNU autoconf test programs,
// but can provide usefull informations about Lisle in your code.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include <lisle/lisle>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

namespace _lisle_ {

#ifdef HAVE_CONFIG_H
static const char* package = PACKAGE;
static const char* version = VERSION;
#else
static const char* package = "lisle";
static const char* version = "1.0.0";
#endif
static const char* name = "Lisle library";
static const char* url = "http://lyric.sourceforge.net";

}

namespace lisle {

std::string package ()
{
	return _lisle_::package;
}

/*
Version version ()
{
	Version version;
  
	version << _lisle_::version;
	return version;
}
*/

std::string name ()
{
	return _lisle_::name;
}

std::string url ()
{
	return _lisle_::url;
}

}
