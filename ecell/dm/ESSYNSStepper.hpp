//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//        This file is part of E-CELL Simulation Environment package
//
//                Copyright (C) 2002 Keio University
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

#ifndef __ESSYNS_HPP
#define __ESSYNS_HPP


// #include <iostream>

#include "libecs/DifferentialStepper.hpp"
#include "ESSYNSProcess.hpp"

USE_LIBECS;

LIBECS_DM_CLASS( ESSYNSStepper, AdaptiveDifferentialStepper )
{

public:

  LIBECS_DM_OBJECT( ESSYNSStepper, Stepper )
    {
      INHERIT_PROPERTIES( AdaptiveDifferentialStepper );
      PROPERTYSLOT_SET_GET( Int, TaylorOrder );
    }

  GET_METHOD( Int, TaylorOrder )
    {
      return theTaylorOrder;
    }

  SET_METHOD( Int, TaylorOrder )
    {
      theTaylorOrder = value;
    }

  ESSYNSStepper()
    :
    theESSYNSProcessPtr( NULLPTR ),
    theTaylorOrder( 1 )
    {
      ; 
    }
	    
  virtual ~ESSYNSStepper()
    {
      ;
    }

  virtual void initialize();
  virtual bool calculate();
    

  virtual Int const getOrder() const 
    {
      return theTaylorOrder;
    }

protected:

  Int theSystemSize;
  Int theTaylorOrder;
  ESSYNSProcessPtr   theESSYNSProcessPtr;
  std::vector<RealVector> theESSYNSMatrix;


  //  RealVector theK1;
};

#endif /* __ESSYNS_HPP */