/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2003-2012, Markus Schütz
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
// Posix signal handling.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include <signals.h>
#include "signal"
#include <lisle/self>
#include <csignal>

namespace lisle {
namespace sys {

static void waitrestart (int)
{
	threadle self(lisle::self);
	sigset_t mask;
	int signal;
	pthread_sigmask(SIG_SETMASK, NULL, &mask);
	sigaddset(&mask, LISLE_SIG_RESTART);
	sigwait(&mask, &signal);
	self->state = thread::running;
}

static void signaled (int signal)
{
	threadle self(lisle::self);
	self->signal = signal;
}

/// This is to be done only once when the signal initialization is called
static void initonce ()
{
	struct sigaction action;
	sigfillset(&action.sa_mask);
	action.sa_flags = 0;
	action.sa_handler = signaled;
	sigaction(LISLE_SIG_RESTART, &action, NULL);
	sigfillset(&action.sa_mask);
	action.sa_flags = SA_NODEFER;
	action.sa_handler = waitrestart;
	sigaction(LISLE_SIG_SUSPEND, &action, NULL);
}

/// This is to be done each time the signal initialization is called
static void initeach ()
{
	sigset_t mask;
	pthread_sigmask(SIG_SETMASK, NULL, &mask);
	sigaddset(&mask, LISLE_SIG_RESTART);   // Block restart signal
	sigdelset(&mask, LISLE_SIG_SUSPEND);   // Unblock suspend signal
	pthread_sigmask(SIG_SETMASK, &mask, NULL);
}

siginit::siginit (void(* once)(), void(* each)())
: base(once)
, init(each)
{}

void siginit::once ()
{
	base::run();
	(*init.each)();
}

void siginit::each ()
{
	(*init.each)();
}

class siginit siginit(initonce, initeach); // Singleton: initializes the signal system

}
}
