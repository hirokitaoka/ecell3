//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//        This file is part of E-CELL Simulation Environment package
//
//                Copyright (C) 2001-2002 Keio University
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
// written by Gabor Bereczki <gabor.bereczki@talk21.com>
// 25/03/2002


#if !defined(__DATAPOINT_HPP)
#define __DATAPOINT_HPP

#include "libecs.hpp"


#include "Polymorph.hpp"

namespace libecs
{

  /** @addtogroup logging
   *@{
   */

  /** \file */



  /**

  */

  class DataPoint
  {


  public:

    enum 
      {  
	TIME_OFFSET = 0,
	VALUE_OFFSET,
	AVG_OFFSET,
	MIN_OFFSET,
	MAX_OFFSET,
	DATAPOINT_LENGTH 
      };


    DataPoint()
      :
      theTime ( 0.0 ), 
      theValue( 0.0 ), 
      theAvg  ( 0.0 ), 
      theMin  ( 0.0 ),
      theMax  ( 0.0 )      
    {
      ; // do nothing
    }

    DataPoint( RealCref aTime, RealCref aValue, 
	       RealCref anAvg, RealCref aMin, RealCref aMax )
      : 
      theTime ( aTime ), 
      theValue( aValue ), 
      theAvg  ( anAvg ), 
      theMin  ( aMin ),
      theMax  ( aMax )      
    {
      ; //do nothing
    }

    ~DataPoint()
    {
      ; // do nothing
    }

    RealCref getTime() const
    {
      return theTime;
    }

    RealCref getValue() const
    {
      return theValue;
    }

    RealCref getAvg() const
    {
      return theAvg;
    }

    RealCref getMin() const
    {
      return theMin;
    }

    RealCref getMax() const
    {
      return theMax;
    }


    void setTime( RealCref aReal )
    {
      theTime = aReal;
    }

    void setValue( RealCref aReal )
    {
      theValue = aReal;
    }

    void setAvg( RealCref aReal )
    {
      theAvg = aReal;
    }

    void setMin( RealCref aReal )
    {
      theMin = aReal;
    }

    void setMax( RealCref aReal )
    {
      theMax = aReal;
    }

    static const size_t getElementSize()
    {
      return sizeof( Real );
    }
    
  protected:

    Real theTime;
    Real theValue;
    Real theAvg;
    Real theMin;
    Real theMax;

  };


  /**

  @internal 

  */

  class DataInterval 
  {
    
  public:

    DataInterval() 
      : 
      theDataPoint(),
      theInterval( -1.0 )
    {
      ; //do nothing
    }

    DataPointCref getFinalDataPoint()
    {
      doPendingCalculations(); //calculate correct AVG 
      return theDataPoint;
    }


    DataPointCref getDataPoint() const
    {
      return theDataPoint;
    }

    DataPointRef getDataPoint()
    {

      return theDataPoint;
    }

    RealCref getInterval() const
    {
      return theInterval;
    }

    RealCref getTime() const
    {
      return getDataPoint().getTime();
    }

    RealCref getValue() const
    {
      return getDataPoint().getValue();
    }

    RealCref getAvg() const
    {
      return getDataPoint().getAvg();
    }

    RealCref getMin() const
    {
      return getDataPoint().getMin();
    }

    RealCref getMax() const
    {
      return getDataPoint().getMax();
    }

    void beginNewInterval()
    {
      theInterval = -1.0;
    }
    
    void addPoint( RealCref aTime, RealCref aValue ) 
    {
      if ( theInterval < 0 ) //the very first time data is added
	{
	  theDataPoint.setTime ( aTime );
	  theDataPoint.setValue( aValue );
	  theDataPoint.setAvg  ( aValue );
	  theDataPoint.setMin  ( aValue );
	  theDataPoint.setMax  ( aValue );
	  theInterval            = 0.0;
	  theLastNonzeroInterval = theInterval;
	  theLastNonzeroValue    = aValue;    
	}
      else
	{
	  const Real aNewInterval( aTime - getTime() );

	  /*
	    theDataPoint.setAvg( getAvg() * theInterval + 
	    aValue * aNewInterval );
	    theDataPoint.setAvg( getAvg() / getInterval() );
	  */

	  //if newinterval is nonzero, calculate previous AVG
	  if ( aNewInterval > 0.0 )
	    {
	      doPendingCalculations();
	      theLastNonzeroInterval = aNewInterval;
	    }

	  theLastNonzeroValue = aValue;

	  if ( aValue < getMin() ) 
	    { 
	      theDataPoint.setMin( aValue ); 
	    }
	  else if ( aValue > getMax() )  // assume theMax > theMin
	    { 
	      theDataPoint.setMax( aValue );
	    }

	  theDataPoint.setValue( aValue );
	  theDataPoint.setTime( aTime );
	  theInterval += aNewInterval;
	  
	}
    }

    void aggregatePoint( DataPointCref aDataPoint, RealCref anInterval )
    {
      if ( theInterval < 0 ) //the very first time data is added
	{
	  theDataPoint           = aDataPoint;
	  theInterval            = anInterval;
	  theLastNonzeroInterval = anInterval;
	  theLastNonzeroValue    = aDataPoint.getAvg();
	}
      else
	{
	  if( aDataPoint.getMin() < getMin() ) 
	    { 
	      theDataPoint.setMin( aDataPoint.getMin() );
	    }
	  if ( aDataPoint.getMax() > getMax() ) 
	    { 
	      theDataPoint.setMax( aDataPoint.getMax() );
	    }

	  //call doPendingCalculations() if anInterval is nonzero
	  if ( anInterval > 0 ) 
	    {
	      doPendingCalculations();
	      theLastNonzeroInterval=anInterval;
	    }

	  theLastNonzeroValue=aDataPoint.getAvg();
	  theDataPoint.setValue( aDataPoint.getValue() );
	  theDataPoint.setTime( getTime() + anInterval );
	  theInterval += anInterval;
	}
    }
  
  private:

    void doPendingCalculations()
    {
      // perform AVG calculations for last nonzero interval
      // 1. last nonzero interval is nonzero, perform calculations normally
      if ( ( theLastNonzeroInterval > 0 ) && ( getInterval() > 0 ) )
	{
	  theDataPoint.
	    setAvg( getAvg() * ( getInterval() - theLastNonzeroInterval ) + 
		    ( theLastNonzeroValue * theLastNonzeroInterval ) );
	  theDataPoint.setAvg( getAvg() / ( getInterval() ) );
	}
      // 2. last nonzero interval is zero, total interval nonzero, 
      //    AVG left unchg
      
      // 3. last nonzero interval is zero, total interval zero, AVG=VAL
      if ( ( theLastNonzeroInterval == 0 ) && ( getInterval() == 0 ) )
	{
	  theDataPoint.setAvg ( theLastNonzeroValue );
	}
      
    }
    
    DataPoint theDataPoint;
    Real      theInterval;
    Real 	theLastNonzeroInterval;
    Real	theLastNonzeroValue;

  };


  //@}

} // namespace libecs


#endif /* __DATAPOINT_HPP */
