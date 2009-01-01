//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//       This file is part of the E-Cell System
//
//       Copyright (C) 1996-2009 Keio University
//       Copyright (C) 2005-2008 The Molecular Sciences Institute
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//
// E-Cell System is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// E-Cell System is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with E-Cell System -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
//END_HEADER
//
// written by Masayuki Okayama <smash@e-cell.org>,
// E-Cell Project.
//
// modified by Gabor Bereczki <gabor.bereczki@talk21.com>
// 14/04/2002
//
// modified by Koichi Takahashi <shafi@e-cell.org>

#include "libecs.hpp"
#include "Polymorph.hpp"

#include "Logger.hpp"

namespace libecs
{

// Constructor
Logger::Logger( LoggerAdapterPtr aLoggerAdapter, Policy const& aPolicy )
    : theLoggerAdapter( aLoggerAdapter ),
      theLastTime( 0.0 ),
      theStepCounter( 0 ),
      thePolicy( aPolicy )
{
    thePhysicalLogger.setEndPolicy( aPolicy.doesContinueOnError() );
    thePhysicalLogger.setMaxSize( aPolicy.getMaxSpace() );
}

//Destructor
Logger::~Logger()
{
    delete theLoggerAdapter;
}

void Logger::setLoggerPolicy( Policy const& aPolicy )
{
    thePolicy = aPolicy;
    thePhysicalLogger.setEndPolicy( aPolicy.doesContinueOnError() );
    thePhysicalLogger.setMaxSize( aPolicy.getMaxSpace() );
}

Logger::Policy const& Logger::getLoggerPolicy( void )
{
    return thePolicy;
}


DataPointVectorSharedPtr Logger::getData( void ) const
{
    if( thePhysicalLogger.empty() )
    {
        return createEmptyVector();
    }
    return thePhysicalLogger.getVector( thePhysicalLogger.begin(),
                                        thePhysicalLogger.end() );
}



DataPointVectorSharedPtr Logger::getData( RealParam aStartTime,
                                          RealParam anEndTime ) const
{
    if( thePhysicalLogger.empty() )
    {
        return createEmptyVector();
    }
    
    PhysicalLogger::size_type topIterator(
            thePhysicalLogger.upper_bound(
                thePhysicalLogger.begin(),
                thePhysicalLogger.end(), 
                anEndTime ) );
    
    PhysicalLogger::size_type bottomIterator(
            thePhysicalLogger.lower_bound(
                thePhysicalLogger.begin(),
                topIterator,
                aStartTime ) );

    return thePhysicalLogger.getVector( bottomIterator, topIterator );
}


DataPointVectorSharedPtr Logger::createEmptyVector()
{
    DataPointVectorSharedPtr aDataPointVector( new DataPointVector ( 0, 2 ) );

    return aDataPointVector;
}

void Logger::log( RealParam aTime )
{
    ++theStepCounter;
    if ( ( theStepCounter >= thePolicy.getMinimumStep()
           && thePolicy.getMinimumTimeInterval() == 0.0 )
         || ( thePolicy.getMinimumTimeInterval() > 0.0 &&
              thePolicy.getMinimumTimeInterval() <=
                  ( aTime - theLastTime ) ) )
    {
        pushData( aTime, theLoggerAdapter->getValue() );
        
        theLastTime = aTime;
        theStepCounter = 0;
    }
}


const Real Logger::getStartTime( void ) const
{
    return  thePhysicalLogger.front().getTime();
}


const Real Logger::getEndTime( void ) const
{
    return thePhysicalLogger.back().getTime();
}

DataPointVectorSharedPtr Logger::getData( RealParam aStartTime,
                                          RealParam anEndTime,
                                          RealParam anInterval ) const
{
    if ( thePhysicalLogger.empty() )
    {
        return createEmptyVector();
    }

    // this case doesn't work well with below routine on x86-64.
    // anyway a serious effort of code cleanup is necessary. 
    if( aStartTime == anEndTime )    
    {
        PhysicalLogger::size_type 
            anIterator( thePhysicalLogger.upper_bound
                                     ( thePhysicalLogger.begin(),
                                         thePhysicalLogger.end(), 
                                         anEndTime ) );
        LongDataPoint 
            aLongDataPoint( thePhysicalLogger.at( anIterator ) );
        DataPointVectorPtr 
            aDataPointVector( new DataPointVector( 1, 5 ) );
        aDataPointVector->asLong( 0 ) = aLongDataPoint;
        return DataPointVectorSharedPtr( aDataPointVector ); 
    }

    // set up output vector
    DataPointVectorIterator aPhysicalRange(
        static_cast<size_type>( ( anEndTime - aStartTime ) / anInterval ) );

    //this is a technical adjustment, because I realized that sometimes
    //conversion from real is flawed: rounding error
    Real anEstimatedRange( ( anEndTime - aStartTime ) / anInterval );

    if ( ( static_cast<Real>(aPhysicalRange) ) + 0.9999 < anEstimatedRange ) 
    {
        ++aPhysicalRange;
    }

    Real aTimeGap( ( thePhysicalLogger.back().getTime() 
                     - thePhysicalLogger.front().getTime() ) /
                                 thePhysicalLogger.size() );

    DataPointVectorPtr aDataPointVector(
        new DataPointVector( aPhysicalRange, 5 ) );

    // set up iterators
    PhysicalLogger::size_type 
        anIterationEnd( thePhysicalLogger.
                        upper_bound_linear_estimate(
                            thePhysicalLogger.begin(),
                            thePhysicalLogger.end(),
                            anEndTime,
                            aTimeGap ) );
    
    PhysicalLogger::size_type 
        anIterationStart( thePhysicalLogger.
                          lower_bound_linear_estimate(
                            thePhysicalLogger.begin(),
                            anIterationEnd,
                            aStartTime,
                            aTimeGap ) );

    // start from vectorslice start to vectorslice end, 
    // scan through all datapoints
    size_type aCounter( anIterationStart );
            
    Real aTargetTime( aStartTime + anInterval );
    LongDataPoint aLongDataPoint( thePhysicalLogger.at( aCounter ) );
    
    DataPointAggregator anAggregator;
    anAggregator.aggregate( aLongDataPoint );
    for ( DataPointVectorIterator anElementCount( 0 ); 
          anElementCount < aPhysicalRange; ++anElementCount )
    {
        do 
        {
            if ( ( aCounter < anIterationEnd ) && 
                 ( aLongDataPoint.getTime() < aTargetTime ) )
            { 
                ++aCounter;
                aLongDataPoint = thePhysicalLogger.at( aCounter );
            }

            anAggregator.aggregate( aLongDataPoint );
        }
        while ( ( aLongDataPoint.getTime() < aTargetTime ) && 
                ( aCounter < anIterationEnd ) );
        
        aDataPointVector->asLong( anElementCount ) = anAggregator.getData();
        anAggregator.beginNextPoint();
        
        aTargetTime += anInterval;
    }
    
    return DataPointVectorSharedPtr( aDataPointVector ); 
}


} // namespace libecs

