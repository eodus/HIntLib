/*
 *  HIntLib  -  Library for High-dimensional Numerical Integration
 *
 *  Copyright (C) 2002  Rudolf Sch�rer <rudolf.schuerer@sbg.ac.at>
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

#ifndef GALOIS_FIELD_H
#define GALOIS_FIELD_H 1

#ifdef __GNUG__
#pragma interface
#endif

#include <iosfwd>

#include <HIntLib/polynomial.h>
#include <HIntLib/modulararithmetic.h>


namespace HIntLib
{

/**
 *  Galois Field
 */

template <class B>
class GaloisField
 : public ModularArithmeticField<PolynomialRingField<ModularIntegerField<B> > >
{
public:
   GaloisField (unsigned base, unsigned exponent);
private:
   typedef ModularIntegerField<B> Field;
   typedef PolynomialRingField<Field> Poly;
   typedef typename Poly::type T;

   T findPoly (unsigned base, unsigned exponent);
};


#if 0
template<class T>
std::ostream& operator<< (std::ostream &, const GaloisField<T> &);
#endif

}  // namespace HIntLib

#endif
