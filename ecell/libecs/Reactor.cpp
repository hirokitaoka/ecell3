//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//        This file is part of E-CELL Simulation Environment package
//
//                Copyright (C) 1996-2002 Keio University
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//
// E-CELL is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// E-CELL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with E-CELL -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
//END_HEADER
//
// written by Kouichi Takahashi <shafi@e-cell.org> at
// E-CELL Project, Lab. for Bioinformatics, Keio University.
//

#include "Util.hpp"
#include "Reactant.hpp"
#include "Stepper.hpp"
#include "FullID.hpp"
#include "Substance.hpp"
#include "Model.hpp"
#include "PropertySlotMaker.hpp"

#include "Reactor.hpp"


namespace libecs
{

  void Reactor::makeSlots()
  {
    registerSlot( getPropertySlotMaker()->
		  createPropertySlot( "Reactant", *this, 
				      Type2Type<PolymorphVectorRCPtr>(),
				      &Reactor::setReactant,
				      NULLPTR ) );

    registerSlot( getPropertySlotMaker()->
		  createPropertySlot( "ReactantList", *this, 
				      Type2Type<PolymorphVectorRCPtr>(),
				      NULLPTR,
				      &Reactor::getReactantList ) );

    registerSlot( getPropertySlotMaker()->
		  createPropertySlot( "Activity", *this, 
				      Type2Type<Real>(),
				      &Reactor::setActivity,
				      &Reactor::getActivity ) );

    registerSlot( getPropertySlotMaker()->
		  createPropertySlot( "ActivityPerSecond", *this,
				      Type2Type<Real>(),
				      NULLPTR,
				      &Reactor::getActivityPerSecond ) );
  }

  const Real Reactor::getActivityPerSecond() const
  {
    return getActivity() * getStepper()->getStepsPerSecond();
  }

  void Reactor::setReactant( PolymorphVectorRCPtrCref aValue )
  {
    checkSequenceSize( *aValue, 3 );

    registerReactant( (*aValue)[0].asString(), 
		      FullID( (*aValue)[1].asString() ), 
		      (*aValue)[2].asInt() );
  }

  const PolymorphVectorRCPtr Reactor::getReactantList() const
  {
    PolymorphVectorRCPtr aVectorPtr( new PolymorphVector );
    aVectorPtr->reserve( theReactantMap.size() );
  
    for( ReactantMapConstIterator i( theReactantMap.begin() );
	 i != theReactantMap.end() ; ++i )
      {
	aVectorPtr->push_back( i->second.getSubstance()->getFullID().getString() );
      }

    return aVectorPtr;
  }

  void Reactor::registerReactant( StringCref aName, FullIDCref aFullID, 
				  const Int aStoichiometry )
  {
    SystemPtr aSystem( getModel()->getSystem( aFullID.getSystemPath() ) );
    SubstancePtr aSubstance( aSystem->getSubstance( aFullID.getID() ) );

    registerReactant( aName, aSubstance, aStoichiometry );
  }

  Reactor::Reactor() 
    :
    theActivity( 0.0 )
  {
    makeSlots();
  }

  Reactor::~Reactor()
  {
    ; // do nothing
  }


  void Reactor::registerReactant( StringCref aName, SubstancePtr aSubstance, 
				  const Int aStoichiometry )
  {
    Reactant aReactant( aSubstance, aStoichiometry );
    theReactantMap.insert( ReactantMap::value_type( aName, aReactant ) );
  }


  Reactant Reactor::getReactant( StringCref aName )
  {
    ReactantMapConstIterator anIterator( theReactantMap.find( aName ) );

    if( anIterator == theReactantMap.end() )
      {
	THROW_EXCEPTION( NotFound,
			 "[" + getFullID().getString() + 
			 "]: Reactant [" + aName + 
			 "] not found in this Reactor." );
      }

    return ( *anIterator ).second;
  }

  void Reactor::initialize()
  {
    ; // do nothing
  }

  PropertySlotPtr 
  Reactor::getPropertySlotOfReactant( StringCref aReactantName,
				      StringCref aPropertyName )
  {
    return getReactant( aReactantName ).
      getSubstance()->getPropertySlot( aPropertyName, this );
  }


  void SRMReactor::makeSlots()
  {
    registerSlot( getPropertySlotMaker()->
		  createPropertySlot( "Priority", *this, 
				      Type2Type<Real>(), // Int?
				      &SRMReactor::setPriority,
				      &SRMReactor::getPriority ) );
  }

  SRMReactor::SRMReactor() 
    :
    thePriority( 0 )
  {
    makeSlots();  
  } 

  SRMReactor::~SRMReactor()
  {
    ; // do nothing
  }

  void SRMReactor::initialize()
  {
    Reactor::initialize();
  }


} // namespace libecs


/*
  Do not modify
  $Author$
  $Revision$
  $Date$
  $Locker$
*/
