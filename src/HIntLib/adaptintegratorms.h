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

/**
 *  AdaptIntegratorMS
 *
 *  Parallel Adaptive Integration using a Master-Slave approach
 */

#ifndef HINTLIB_ADAPT_INTEGRATOR_MS_H
#define HINTLIB_ADAPT_INTEGRATOR_MS_H

#include <HIntLib/defaults.h>

#ifndef HINTLIB_PARALLEL
#error AdaptIntegratorMS can only be used in PARALLEL mode
#endif

#ifdef HINTLIB_USE_INTERFACE_IMPLEMENTATION
#pragma interface
#endif

#include <HIntLib/rulebasedintegrator.h>
#include <HIntLib/regioncollection.h>

namespace HIntLib
{

   class EmbeddedRuleFactory;

class AdaptIntegratorMS : public EmbeddedRuleBasedIntegrator
{
public:

   virtual
   Status integrate (
      Integrand &f, const Hypercube &h, Index maxEval,
      real reqAbsError, real reqRelError, EstErr &ee);

   Index getFrequency() const  { return frequency; }
   void  setFrequency(Index f)  { frequency = f; }

protected:

   AdaptIntegratorMS (const EmbeddedRuleFactory *, MPI_Comm);

   unsigned numWorkers (void) const;

   void slave (
      Integrand &, const Hypercube &, EmbeddedRule &, unsigned interval);

   virtual Status master (const Hypercube &,
      real reqAbsError, real reqRelError, EstErr &ee, Index maxIter) = 0;

   void sendTerminationSignal ();
   void sendTopRegionToSlave (
         RegionCollection &, int worker, EstErr*,
         RegionCollection::size_type n = 1);
   void sendTopRegionToAllSlaves (
         RegionCollection &, EstErr*, RegionCollection::size_type n = 1);
   int recvRegionsFromSlave (RegionCollection &, int worker, unsigned dim,
                             EstErr* localEE);
   void recvRegionsFromAllSlaves (RegionCollection &, unsigned dim,
                             EstErr* localEE);

   // Variables

   Index frequency;
   MPI_Comm comm;

   int rank;
   int nodes;

   Index interval;
   unsigned numSentBack;
};

class AdaptIntegratorMSSync : public AdaptIntegratorMS
{
public:
   AdaptIntegratorMSSync (
         const EmbeddedRuleFactory *rf, MPI_Comm comm = MPI_COMM_WORLD)
      : AdaptIntegratorMS (rf, comm)  {}

protected:
   virtual Status master (const Hypercube &,
      real reqAbsError, real reqRelError, EstErr &ee, Index maxIter);
};

class AdaptIntegratorMSAsync : public AdaptIntegratorMS
{
public:
   AdaptIntegratorMSAsync (
         const EmbeddedRuleFactory *rf, MPI_Comm comm = MPI_COMM_WORLD)
      : AdaptIntegratorMS (rf, comm)  {}

protected:
   virtual Status master (const Hypercube &,
      real reqAbsError, real reqRelError, EstErr &ee, Index maxIter);
};

class AdaptIntegratorMSInter : public AdaptIntegratorMS
{
public:
   AdaptIntegratorMSInter (
         const EmbeddedRuleFactory *rf, MPI_Comm comm = MPI_COMM_WORLD)
      : AdaptIntegratorMS (rf, comm)  {}

protected:
   virtual Status master (const Hypercube &,
      real reqAbsError, real reqRelError, EstErr &ee, Index maxIter);
};

}  // namespace HIntLib

#endif

