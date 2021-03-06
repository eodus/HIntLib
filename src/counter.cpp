/*
 *  HIntLib  -  Library for High-dimensional Numerical Integration
 *
 *  Copyright (C) 2002,03,04,05  Rudolf Schuerer <rudolf.schuerer@sbg.ac.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#define HINTLIB_LIBRARY_OBJECT

#include <HIntLib/counter.h>

#ifdef HINTLIB_USE_INTERFACE_IMPLEMENTATION
#pragma implementation
#endif

#include <HIntLib/output.h>

namespace L = HIntLib;

std::ostream& L::operator<< (std::ostream& o, const CounterBase & c)
{
   Private::Printer ss (o);
   for (const unsigned* p = c.array() + c.getNumDigits() - 1;
        p >= c.array(); --p)
   {
      ss << *p;
   }

   return o;
}
#ifdef HINTLIB_BUILD_WCHAR
std::wostream& L::operator<< (std::wostream& o, const CounterBase & c)
{
   Private::WPrinter ss (o);
   for (const unsigned* p = c.array() + c.getNumDigits() - 1;
        p >= c.array(); --p)
   {
      ss << *p;
   }

   return o;
}
#endif

void
L::CounterBase::reset()
{
   std::fill (index.begin(), index.begin() + digits, 0);
}

bool
L::Counter::next()
{
   for (unsigned i = 0; i < digits; ++i)
   {
      if (index[i] < baseMinusOne)
      {
         ++index[i];
         return true;
      }
      else
      {
         index[i] = 0;
      }
   }

   return false;
}

bool
L::CounterMixedBase::next()
{
   for (unsigned i = 0; i < digits; ++i)
   {
      if (index[i] < baseMinusOne[i])
      {
         ++index[i];
         return true;
      }
      else
      {
         index[i] = 0;
      }
   }

   return false;
}

bool
L::GrayCounter::next()
{
   for (unsigned i = 0; i < digits; ++i)
   {
      unsigned& pos = index[i];
      int&      dir = direction[i];

      if (   (pos != baseMinusOne && dir == 1) || (pos == 0 && dir == -1))
      {
         pos += dir;
         return true;
      }
      else
      {
         dir = - dir;
      }
   }

   return false;
}

unsigned
L::GrayCounter::nextDigit()
{
   for (unsigned i = 0; i < digits; ++i)
   {
      unsigned& pos = index[i];
      int&      dir = direction[i];

      if (   (pos != baseMinusOne && dir == 1) || (pos == 0 && dir == -1))
      {
         pos += dir;
         return i;
      }
      else
      {
         dir = - dir;
      }
   }

   return digits;
}

unsigned
L::GrayCounter::nextDigit(unsigned* newDigit)
{
   for (unsigned i = 0; i < digits; ++i)
   {
      unsigned& pos = index[i];
      int&      dir = direction[i];

      if (   (pos != baseMinusOne && dir == 1) || (pos != 0 && dir == -1))
      {
         *newDigit = pos += dir;
         return i;
      }
      else
      {
         dir = - dir;
      }
   }

   return digits;
}

void
L::GrayCounter::reset()
{
   CounterBase::reset();

   std::fill (direction.begin(), direction.begin() + digits, 1);
}

