/*
 *  HIntLib  -  Library for High-dimensional Numerical Integration
 *
 *  Copyright (C) 2002,03,04,05  Rudolf Sch�rer <rudolf.schuerer@sbg.ac.at>
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

#ifdef __GNUG__
#pragma implementation
#endif

#define HINTLIB_LIBRARY_OBJECT

#include <algorithm>

#include <HIntLib/generatormatrixgenrow.h>
#include <HIntLib/exception.h>


namespace L = HIntLib;

using std::numeric_limits;


/**
 *  assign Matrix
 *
 *  Assigns a submatrix of a Generator Matrix to another Generator Matrix
 */

template<class T>
void L::assign (
   const GeneratorMatrix & src, unsigned srcDim,
         GeneratorMatrixGenRow<T> & dst, unsigned dstDim)
{
#if 0
   cerr <<"A3" <<endl;
   cerr << "dim = " << srcM.getDimension() << " "<<dstM.getDimension() << endl;
   cerr << "m = " << srcM.getM() << " "<<dstM.getM() << endl;
   cerr << "prec = " << srcM.getPrec() << " "<<dstM.getPrec() << endl;
   cerr << "base = " << srcM.getBase() << " "<<dstM.getBase() << endl;
#endif

   GeneratorMatrix::checkCopy (src, dst);

   if (dstDim >= dst.getDimension())
   {
      throw DimensionTooHigh (dstDim, dst.getDimension() - 1);
   }

   if (srcDim >= src.getDimension())
   {
      throw DimensionTooHigh (dstDim, dst.getDimension() - 1);
   }

   T* p = dst(dstDim);

   for (unsigned b = 0; b < dst.getPrec(); ++b)
   {
      for (unsigned r = 0; r < dst.getM(); ++r)
      {
         *p++ = src.getDigit (srcDim, r, b);
      }
   }
}

template<class T>
void L::assign (const GeneratorMatrix &src, GeneratorMatrixGenRow<T> &dst)
{
   for (unsigned d = 0; d < dst.getDimension(); ++d)  assign (src, d, dst, d);
}


/*****************************************************************************/
/***     Generator Matrix Gen Row Base                                     ***/
/*****************************************************************************/

L::GeneratorMatrixGenRowBase::GeneratorMatrixGenRowBase (
      const GeneratorMatrix& gm)
   : GeneratorMatrix (
        gm.getBase(),
        gm.getDimension(),
        gm.getM(),
        gm.getPrec()),
     mPrec (m * prec),
     linAlg (LinearAlgebra::make (base))
{}


/*****************************************************************************/
/***     Generator Matrix Gen <T>                                          ***/
/*****************************************************************************/

/**
 *  Constructor
 */

template<class T>
L::GeneratorMatrixGenRow<T>::GeneratorMatrixGenRow (unsigned _base, unsigned _dim)
   : GeneratorMatrixGenRowBase (
      _base, _dim, getDefaultM (_base), getDefaultPrec (_base))
{
   checkBase();
   allocate();
   makeZeroMatrix();
}

template<class T>
L::GeneratorMatrixGenRow<T>::GeneratorMatrixGenRow
      (unsigned _base, unsigned _dim, unsigned _m)
   : GeneratorMatrixGenRowBase (
      _base, _dim, _m, std::min (_m, getDefaultPrec (_base)))
{
   checkBase();
   allocate();
   makeZeroMatrix();
}


template<class T>
L::GeneratorMatrixGenRow<T>::GeneratorMatrixGenRow
      (unsigned _base, unsigned _dim, unsigned _m, unsigned _prec)
   : GeneratorMatrixGenRowBase (_base, _dim, _m, _prec)
{
   checkBase();
   allocate();
   makeZeroMatrix();
}


/**
 *  Copy constructor
 */

template<class T>
L::GeneratorMatrixGenRow<T>::GeneratorMatrixGenRow (
      const GeneratorMatrixGenRow<T> &gm)
   : GeneratorMatrixGenRowBase (gm)
{
   allocate();
   std::copy (gm.getMatrix(), gm.getMatrix() + dim * mPrec, c);
}


/**
 *  Copy from arbitrary Generator Matrix
 */

template<class T>
L::GeneratorMatrixGenRow<T>::GeneratorMatrixGenRow (const GeneratorMatrix &gm)
   : GeneratorMatrixGenRowBase (gm)
{
   checkBase();
   allocate();
   for (unsigned d = 0; d < dim; ++d)  assign (gm, d, *this, d);
}


/**
 *  checkBase()
 */

template<class T>
void L::GeneratorMatrixGenRow<T>::checkBase() const
{
   if (getBase() < 2 || getBase() - 1 > numeric_limits<T>::max())
   {
      throw GM_BaseTooLarge (getBase(), unsigned (numeric_limits<T>::max()));
   }
}


/**
 *  allocate()
 */

template<class T>
void L::GeneratorMatrixGenRow<T>::allocate()
{
   c = new T [mPrec * dim];
}


/**
 *  makeZeroColumnVector ()
 */

template<class T>
void L::GeneratorMatrixGenRow<T>::makeZeroColumnVector (unsigned d, unsigned r)
{
   T* i = c + d * mPrec + r;
   T* end = i + mPrec;

   while (i != end)
   {
      *i = 0;
      i += m;
   }
}


/**
 *  getPackedRowVector ()
 */

template<class T>
L::u64
L::GeneratorMatrixGenRow<T>::getPackedRowVector (unsigned d, unsigned b) const
{
   u64 result (0);
   const T* end = (*this)(d,b);
   const T* i = end + m;
   
   while (i != end)  result = result * base + (*--i);

   return result;
}


/**
 *  setPackedRowVector ()
 */

template<class T>
void
L::GeneratorMatrixGenRow<T>::setPackedRowVector (unsigned d, unsigned b, u64 x)
{
   T* i = c + d * mPrec + b * m;
   T* end = i + m;

   while (i != end)
   {
      *i++ = x % base;

      if (! (x /= base))
      {
         std::fill (i, end, 0);
         break;
      }
   }
}


/**
 *  makeZeroRowVector ()
 */

template<class T>
void L::GeneratorMatrixGenRow<T>::makeZeroRowVector (unsigned d, unsigned b)
{
   T* i = c + d * mPrec + b * m;
   std::fill (i, i + m, 0);
}


/**
 *  getDigit ()
 */

template<class T>
unsigned
L::GeneratorMatrixGenRow<T>::getDigit (unsigned d, unsigned r, unsigned b) const
{
   if (numeric_limits<T>::digits > numeric_limits<unsigned>::digits)
   {
      throw InternalError (__FILE__, __LINE__);
   }

   return unsigned((*this) (d, r, b));
}


/**
 *  setDigit ()
 */

template<class T>
void L::GeneratorMatrixGenRow<T>::setDigit
   (unsigned d, unsigned r, unsigned b, unsigned x)
{
   if (   numeric_limits<T>::digits < numeric_limits<unsigned>::digits
       && x >= getBase())
   {
      throw InternalError (__FILE__, __LINE__);
   }

   setd (d, r, b, T(x));
}


/**
 *  vSet/GetPackedRowVector ()
 */

template<typename T>
L::u64
L::GeneratorMatrixGenRow<T>::vGetPackedRowVector (unsigned d, unsigned b) const
{
   return getPackedRowVector (d, b);
}

template<typename T>
void
L::GeneratorMatrixGenRow<T>::vSetPackedRowVector (unsigned d, unsigned b, u64 x)
{
   setPackedRowVector (d, b, x);
}


/**
 *  make Hammersley()
 */

template<class T>
void L::GeneratorMatrixGenRow<T>::makeHammersley (unsigned d)
{
   makeZeroMatrix (d);

   for (unsigned r = 0; r < m; ++r)
   {
      if (m-(r+1) < prec)  setd (d, r, m-(r+1), 1);
   }
}


/**
 *  make Identity Matrix ()
 */

template<class T>
void L::GeneratorMatrixGenRow<T>::makeIdentityMatrix  (unsigned d)
{
   makeZeroMatrix (d);

   const unsigned ub = std::min (m, prec);
   for (unsigned r = 0; r < ub; ++r)  setd (d, r, r, 1);
}


/**
 *  make Zero Matrix ()
 */

template<class T>
void L::GeneratorMatrixGenRow<T>::makeZeroMatrix (unsigned d)
{
   T* i = c + d * mPrec;
   std::fill (i, i + mPrec, 0);
}

template<class T>
void L::GeneratorMatrixGenRow<T>::makeZeroMatrix ()
{
   std::fill (c, c + mPrec * dim, 0);
}


/**
 *  make Shift Net ()
 *
 *  Creates a shift net based on the matrix for dimension 0
 */

template<typename T>
void L::GeneratorMatrixGenRow<T>::makeShiftNet (unsigned b)
{
   for (unsigned r = 0; r < m; ++r)
   {
      T x = (*this)(0, r, b);

      for (unsigned d = 1;       d < dim - r; ++d)  setd (d, d+r,     b, x);
      for (unsigned d = dim - r; d < dim;     ++d)  setd (d, d+r-dim, b, x);
   }
}

template<typename T>
void L::GeneratorMatrixGenRow<T>::makeShiftNet()
{
   for (unsigned b = 0; b < prec; ++b)  makeShiftNet (b);
}


/**
 *  operator==
 */

template<class T>
bool L::operator==
   (const L::GeneratorMatrixGenRow<T> &m1, const L::GeneratorMatrixGenRow<T> &m2)
{
   return m1.getDimension() == m2.getDimension()
       && m1.getM()         == m2.getM()
       && m1.getPrec()      == m2.getPrec()
       && std::equal(
            m1.getMatrix(),
            m1.getMatrix() + m1.getDimension() * m1.getPrec() * m1.getM(),
            m2.getMatrix());
}


namespace HIntLib
{
#define HINTLIB_INSTANTIATE(X) \
   template class GeneratorMatrixGenRow<X>; \
   template void assign ( \
      const GeneratorMatrix &, unsigned, GeneratorMatrixGenRow<X> &, unsigned); \
   template void assign (const GeneratorMatrix &, GeneratorMatrixGenRow<X> &); \
   template bool operator== ( \
      const GeneratorMatrixGenRow<X> &, const GeneratorMatrixGenRow<X> &);

   HINTLIB_INSTANTIATE (unsigned char)
   HINTLIB_INSTANTIATE (unsigned short)
#undef HINTLIB_INSTANTIATE
}


#include <HIntLib/generatormatrixvirtual.h>
#include <HIntLib/prime.h>
#include <HIntLib/lookupfield.h>

/**
 * Virtual Matrix Base
 */

L::VirtualMatrixBase::~VirtualMatrixBase ()
{
   delete gm;
}

void L::VirtualMatrixBase::setMatrix (GeneratorMatrixGenRow<unsigned char>* p)
{
   gm = p;
   setParameters (*gm);
}

unsigned
L::VirtualMatrixBase::getDigit  (unsigned d, unsigned r, unsigned b) const
{
   return (*gm)(d, r, b);
}

L::u64
L::VirtualMatrixBase::vGetPackedRowVector (unsigned d, unsigned b) const
{
   return gm->getPackedRowVector (d, b);
}


/**
 * Base Reduction
 */

L::BaseReduction::BaseReduction (const GeneratorMatrix& src)
{
   const unsigned srcBase = src.getBase();
   unsigned prime, power;
   Prime::factorPrimePower (srcBase, prime, power);

   if (power == 1)
   {
      setMatrix (new GeneratorMatrixGenRow<unsigned char> (src));
      return;
   }

   GeneratorMatrixGenRow<unsigned char>* gm =
      new GeneratorMatrixGenRow<unsigned char> (
         prime, src.getDimension(),
         src.getM() * power,
         src.getPrec() * power);
   setMatrix (gm);

   LookupGaloisField<unsigned char> a (srcBase);

   for (unsigned d = 0; d < src.getDimension(); ++d)
   for (unsigned r = 0; r < src.getM(); ++r)
   for (unsigned p = 0; p < src.getPrec(); ++p)
   {
      unsigned char digit = src.getDigit (d, r, p);

      for (unsigned i = 0; i < power; ++i)
      {
         unsigned char digit1 = a.mul (digit, a.element (powInt (prime, i)));

         for (unsigned j = 0; j < power; ++j)
         {
            gm->setDigit (d, r * power + i, (p+1) * power - 1 - j,
                             digit1 % prime);
            digit1 /= prime;
         }
      }
   }
}

