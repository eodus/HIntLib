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


#ifndef HINTLIB_T_PARAMETER_H
#define HINTLIB_T_PARAMETER_H 1

#include <HIntLib/defaults.h>

#ifdef HINTLIB_USE_INTERFACE_IMPLEMENTATION
#pragma interface
#endif

#include <HIntLib/generatormatrix.h>
#include <HIntLib/lookupfield.h>
#include <HIntLib/linearalgebra.h>
#include <HIntLib/linearalgebra2.h>

namespace HIntLib
{

template<class T> class GeneratorMatrixGen;
template<class T> class GeneratorMatrixGenRow;
template<class T> class GeneratorMatrix2Row;

void makeRegular (GeneratorMatrixGenRow<unsigned char>&, int d);
void fixOneDimensionalProjections (GeneratorMatrixGenRow<unsigned char>&);
void fixTwoDimensionalProjections (GeneratorMatrixGenRow<unsigned char>&);
void withIdentityMatrix (GeneratorMatrixGenRow<unsigned char>&, int d = 0);
void withIdentityMatrix2(GeneratorMatrixGenRow<unsigned char>&, int d = 0);


/***********  TCalc  *********************************************************/

/**
 *  TCalc
 *
 *  Base class for TCalc2 and TCalcGen
 */

class TCalc
{
protected:
   explicit TCalc (int _dim);
   ~TCalc ();

   bool partitionContains (int) const;

   const int dim;
   int *const partition;

private:
   enum { MAX_PARTITION_SIZE = 200 };
   static int staticPartition [MAX_PARTITION_SIZE];
};

inline
bool TCalc::partitionContains (int value) const
{
   for (int d = 0; d < dim; ++d)  if (partition[d] == value)  return true;
   return false;
}


/***********  TCalc2  ********************************************************/


/**
 * TCalc2
 *
 * Contains the infrastructure for calculating the t-parameter in base b=2.
 *
 * This class is not multithreading-save.
 */

template<typename T>
class TCalc2 : public TCalc
{
public:
   TCalc2 (const HIntLib::GeneratorMatrix2Row<T> &_gm)
      : TCalc (_gm.getDimension()), gm(_gm) {}

   bool check (int strength, bool dimOpt);
   bool checkTO (int strength, bool dimOpt);
   bool checkRestricted (int strength, int maxRows);
   bool checkRestrictedRO (int strength, int maxRows);
   bool checkRestrictedTO (int strength, int maxRows);
   bool checkRestrictedTORO (int strength, int maxRows);

private:
   void init (const GeneratorMatrix &);

protected:
   void copyToSelection (int d, int num, T*& pos);
   void copyToSelection ();
   bool singular (T* l)
      { return ! isLinearlyIndependent (&selection[0], l); }
   bool singular (int strength)
      { return ! isLinearlyIndependent (&selection[0], &selection[strength]); }

   const GeneratorMatrix2Row<T> & gm;
   static HINTLIB_DLL_IMPORT T selection [std::numeric_limits<T>::digits];
};


/**
 *  copyToSeclection ()
 */

template<typename T>
inline
void TCalc2<T>::copyToSelection (int d, int num, T*& pos)
{
   for (int i = 0; i < num; ++i)  *(pos++) = gm(d,i);
}

template<typename T>
inline
void TCalc2<T>::copyToSelection ()
{
   T* pos = &selection[0];
   for (int d = 0; d < dim; ++d)  copyToSelection (d, partition[d], pos);
}


/***********  T Calc Gen  ****************************************************/


class TCalcGen : public TCalc
{
public:
   TCalcGen (const GeneratorMatrixGen<unsigned char> &);
   ~TCalcGen ();

   bool check (int strength, bool dimOpt);
   bool checkRestricted (int strength, int maxRows);
   bool checkRestrictedRO (int strength, int maxRows);

protected:
   void copyToSelection (int d, int num, int& pos);
   void copyToSelection ();
   bool singular (int strength, int newM);
   bool singular (int strength);

   // void dumpSelection (std::ostream &) const;

   unsigned char* selection;

   const GeneratorMatrixGen<unsigned char> & gm;
   const int M;
   
private:
   LinearAlgebra* la;

   enum { MAX_SELECTION_SIZE = 1000 };
   static unsigned char staticSelection [MAX_SELECTION_SIZE];

   TCalcGen ();
};


/**
 * singular()
 */

inline
bool TCalcGen::singular (int strength, int newM)
{
   return ! la->isLinearlyIndependent (&selection[0], strength, newM);
}

inline
bool TCalcGen::singular (int strength)
{
   return ! la->isLinearlyIndependent (&selection[0], strength, M);
}


/*****************  Interface to public routines  ****************************/

enum TOption
{
   DEFAULT = 0,
   LOWER_RESTRICTION_OK = 1, // Asuume that any smaller restriction is ok
   LARGER_T_OK = 2,          // Assume that any larger t (lower strength) is ok
   LOWER_DIM_OK = 4          // Assume that its ok without the last matrix
};


/**
 *  tParameter()
 */

int tParameter (const GeneratorMatrix&, int lb, int ub, TOption = DEFAULT);

inline
int tParameter (const GeneratorMatrix& gm, TOption opts = DEFAULT)
{
   return tParameter (gm, 0, gm.getM(), opts);
}

/**
 *  tParameterRestricted()
 */

int tParameterRestricted (
      const GeneratorMatrix&,
      int lb, int ub, int maxRows, TOption opts = DEFAULT);

/**
 *  confirmT()
 */

bool confirmT (const GeneratorMatrix& gm, int t, TOption opts = DEFAULT);

/**
 *  confirmTRestricted ()
 */

bool confirmTRestricted (
      const GeneratorMatrix& gm, int t, int maxRows, TOption opts = DEFAULT);


/*************** t-Parameter of low-dimensionl projections *******************/


int tParameter1DimProjection (const GeneratorMatrix&, int d);
int tParameter2DimProjection (const GeneratorMatrix&, int d1, int d2);

int tParameterMax1DimProjection (const GeneratorMatrix&);
int tParameterMax2DimProjection (const GeneratorMatrix&);
int tParameterMax3DimProjection (const GeneratorMatrix&);

}  // namespace HIntLib

#endif

