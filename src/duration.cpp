/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2003,2005, Markus Schutz
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
#include <lisle/duration.h>
#include <cmath>

#ifdef _MSC_VER
#pragma warning (disable:4244)
#endif

#define MAX_NSEC 999999999

using namespace std;

namespace lisle {

duration::duration (double seconds)
{
	double absd = fabs(seconds);
	data.sec = floor(absd);
	data.nsec = (absd - floor(absd)) * MAX_NSEC;
	if (seconds < 0)
		data.sec = -data.sec;
}

duration duration::operator - () const
{
	duration that(*this);
	that.data.sec = -that.data.sec;
	return that;
}

bool duration::operator == (const duration& that) const
{
	bool res = (this->data.sec == that.data.sec) && (this->data.nsec == that.data.nsec);
	return res;
}

bool duration::operator != (const duration& that) const
{
	bool res = (this->data.sec != that.data.sec) || (this->data.nsec != that.data.nsec);
	return res;
}

bool duration::operator < (const duration& that) const
{
	if (this->data.sec < that.data.sec) return true;
	if (this->data.sec == that.data.sec)
	{
		if (this->data.nsec < that.data.nsec) return true;
	}
	return false;
}

bool duration::operator > (const duration& that) const
{
	if (this->data.sec > that.data.sec) return true;
	if (this->data.sec == that.data.sec)
	{
		if (this->data.nsec > that.data.nsec) return true;
	}
	return false;
}

duration& duration::operator += (const duration& that)
{
	if (that < duration(0))
		*this -= -that;
	else
	{
		this->data.sec += that.data.sec;
		if (that.data.nsec >= MAX_NSEC - this->data.nsec)
		{
			this->data.nsec += that.data.nsec;
			this->data.nsec %= MAX_NSEC;
			this->data.sec++;
		}
		else
		{
			this->data.nsec += that.data.nsec;
		}
	}
	return *this;
}

duration& duration::operator -= (const duration& that)
{
	if (that < duration(0))
		*this += -that;
	else
	{
		if (that.data.sec > this->data.sec)
		{
			this->data.sec = that.data.sec - this->data.sec;
			this->data.nsec = MAX_NSEC - this->data.nsec;
		}
		else
			this->data.sec -= that.data.sec;
		if (that.data.nsec > this->data.nsec)
		{
			this->data.nsec = MAX_NSEC - that.data.nsec + this->data.nsec;
			if (this->data.nsec < 0)
				this->data.sec++;
			else
			{
				if (this->data.sec == 0)
					this->data.sec = 1;
				else
					this->data.sec--;
			}
		}
		else
			this->data.nsec -= that.data.nsec;
	}
	return *this;
}

}

using namespace lisle;

duration operator + (const duration& lho, const duration& rho)
{
	duration result(lho);
	result += rho;
	return result;
}

duration operator - (const duration& lho, const duration& rho)
{
	duration result(lho);
	result -= rho;
	return result;
}
