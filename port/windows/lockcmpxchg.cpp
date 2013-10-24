/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2003, Markus Schütz
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
// InterlockedCompareExchange function for Win95.
// Grabed from the pthread for win32 project.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include "lockcmpxchg"
#include <stdlib.h>
#include <winbase.h>

namespace lisle {

long (WINAPI* lockcmpxchg) (long* location, long value, long comparand) = NULL;

long WINAPI lockcmpxchg_fct (long* location, long value, long comparand)
{
	long result;
	_asm
	{
		push         ecx
		push         edx
		mov          ecx,dword ptr [location]
		mov          edx,dword ptr [value]
		mov          eax,dword ptr [comparand]
		lock cmpxchg dword ptr [ecx],edx         ; if (EAX == [ECX])
		                                         ;   [ECX] = EDX
		                                         ; else
		                                         ;   EAX = [ECX]
		mov          dword ptr [result],eax
		pop          edx
		pop          ecx
	}
	return result;
}

void lockcmpxchg_init ()
{
	HINSTANCE dll;
	dll = LoadLibrary("kernel32.dll");
	lockcmpxchg = (long (WINAPI *)(long*, long, long))GetProcAddress(dll, "InterlockedCompareExchange");
	FreeLibrary(dll);
	if (lockcmpxchg == NULL)
		lockcmpxchg = lockcmpxchg_fct;
}

}
