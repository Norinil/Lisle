// -*- C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005, Markus Schutz
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
// $Id: prime.cpp 3240 2005-01-23 02:52:08Z schutzm $
//
// Project URL: http://lyric.sourceforge.net
//
///////////////////////////////////////////////////////////////////////////////


#include <lisle/Thread>
#include <lisle/Mutex>
#include <lyric/List>
#include <lyric/Types>
#include <lyric/Handle>
#include <lyric/Circreg>
#include <cstdio>


#define NTHREADS 1


class Primes : public List<uint>, public Mutex
{
};

class Number
{
public:
  Number (uint value = 0, bool prime = false)
    : value_(value), prime_(prime) {};
  uint value () const
    { return value_; }
  bool prime () const
    { return prime_; }
private:
  uint value_;
  bool prime_;
};

struct Arg
{
  Arg (Primes* list, Number check)
    : primes(list), number(check) {};
  Primes* primes;
  Number  number;
};


/*===========================================================================*/
Handle<Number> prime (Handle<Arg> arg)
{
  bool    result = true;
  bool    done = false;
  Size    index = 0;
  uint    prime;
  Primes* primes = arg->primes;

  while (!done)
    {
      primes->lock();
      if (index < primes->size())
	prime = (*primes)[index];
      else
	done = true;
      primes->unlock();
      if (arg->number.value() % prime == 0)
	{
	  result = false;
	  done = true;
	}
      index++;
    }
  if (result)
    {
      primes->lock();
      primes->append(arg->number.value());
      primes->unlock();
    }
  return Handle<Number>(new Number(arg->number.value(), result));
}
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
int main ()
{
  using namespace Thread;

  Primes*        primes = new Primes;
  Circreg<Id>    tids(NTHREADS);
  Thread::Id     old;
  Handle<Number> number;
  Size           found = 1;

  primes->append(2);
  printf("%7u ", 2);
  for (Size i=3; i<3+NTHREADS; i++)
    tids.store(create(Start(&prime, Handle<Arg>(new Arg(primes, Number(i))))));
  for (Size i=3+NTHREADS; i<100000; i++)
    {
      old = tids.store(create(Start(&prime,
				    Handle<Arg>(new Arg(primes, Number(i))))));
      old.join(number);
      if (number->prime())
	{
	  if (found % 16 == 0)
	    printf("\n");
	  printf("%7u ", number->value());
	  found++;
	}
    }
  for (Size i=0; i<NTHREADS; i++)
    {
      old = tids.store(Thread::Id());
      old.join(number);
      if (number->prime())
	{
	  if (found % 16 == 0)
	    printf("\n");
	  printf("%7u ", number->value());
	  found++;
	}
    }
  printf("\n");
  printf("Found %u prime numbers\n", found);
  return 0;
}
/*---------------------------------------------------------------------------*/
