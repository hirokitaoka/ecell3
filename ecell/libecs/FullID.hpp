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

#ifndef ___FULLID_HPP
#define ___FULLID_HPP
#include <string>

#include "libecs.hpp"
#include "EntityType.hpp"


namespace libecs
{

  /** @addtogroup identifier The FullID, FullPN and SystemPath.
   The FullID, FullPN and SystemPath.
   

   @ingroup libecs
   @{ 
   */ 
  
  /** @file */


  /** 
      SystemPath 
  */

  class SystemPath : public StringList
  {

  public:

    explicit SystemPath( StringCref systempathstring = "" )
    {
      parse( systempathstring );
    }

    SystemPath( SystemPathCref systempath )
      :
      StringList( static_cast<StringList>( systempath ) )
    {
      ; // do nothing
    }

    ~SystemPath() {}

    const String getString() const;

    bool isAbsolute() const
    {
      return ( ( ( ! empty() ) && ( front()[0] == DELIMITER ) ) || empty() );
    }

    bool isValid() const
    {
      // FIXME: check '..'s and '.'s etc..
      return true;
    }

  protected:

    /**
       Standardize a SystemPath. 
       Reduce '..'s and remove extra white spaces.

       @return reference to the systempath
    */
    void standardize();

  private:

    //    SystemPath();


    void parse( StringCref systempathstring );

  public:

    static const char DELIMITER = '/';

  };


  /**
     FullID is an identifier of a unique Entiy in a cell model.
     The FullID consists of a EntityType, a SystemPath and an ID string.

     @see EntityType, SystemPath
  */
  class FullID
  {

  public:

    FullID( const EntityType type,
	    SystemPathCref systempath,
	    StringCref id )
      :
      theEntityType( type ),
      theSystemPath( systempath ),
      theID( id )
    {
      ; // do nothing
    }

    explicit FullID( const EntityType type,
		     StringCref systempathstring,
		     StringCref id )
      :
      theEntityType( type ),
      theSystemPath( systempathstring ),
      theID( id )
    {
      ; // do nothing
    }

    FullID( StringCref fullidstring )
    {
      parse( fullidstring );
    }

    FullID( FullIDCref fullid )
      :
      theEntityType( fullid.getEntityType() ),
      theSystemPath( fullid.getSystemPath() ),
      theID( fullid.getID() )
    {
      ; // do nothing
    }


    ~FullID() {}
  
    const EntityType  getEntityType() const 
    { 
      return theEntityType; 
    }

    const SystemPathCref getSystemPath()    const 
    { 
      return theSystemPath; 
    }

    const StringCref     getID()            const 
    { 
      return theID;
    }


    void setEntityType( const EntityType type )
    {
      theEntityType = type;
    }

    void setSystemPath( SystemPathCref systempath ) 
    {
      theSystemPath = systempath;
    }

    void setID( StringCref id ) 
    {
      theID = id;
    }

    bool isValid() const;

    const String getString() const;

    bool operator<( FullIDCref rhs ) const
    {
      // first look at the EntityType
      if( getEntityType() != rhs.getEntityType() )
	{
	  return getEntityType() < rhs.getEntityType();
	}

      // then compare the SystemPaths
      // FIXME: should be faster is there is SystemPath::compare()
      if( getSystemPath() != rhs.getSystemPath() )
	{
	  return getSystemPath() < rhs.getSystemPath();
	}

      // finally compare the ID strings
      return getID() < rhs.getID();
    }

    bool operator==( FullIDCref rhs ) const
    {
      // first look at the EntityType
      if( getEntityType() != rhs.getEntityType() )
	{
	  return false;
	}

      // then compare the SystemPaths
      if( getSystemPath() != rhs.getSystemPath() )
	{
	  return false;
	}

      // finally compare the ID strings
      return getID() == rhs.getID();
    }

    bool operator!=( FullIDCref rhs ) const
    {
      return ! operator==( rhs );
    }

  protected:

    void parse( StringCref fullidstring );

  private:

    FullID();

  public:

    static const char DELIMITER = ':';

  private:

    EntityType theEntityType;
    SystemPath    theSystemPath;
    String        theID;

  };

  class FullPN
  {

  public:

    FullPN( const EntityType type, 
	    SystemPathCref systempath,
	    StringCref id,
	    StringCref propertyname )
      :
      theFullID( type, systempath, id ),
      thePropertyName( propertyname )
    {
      ; // do nothing
    }

    FullPN( FullIDCref fullid, StringCref propertyname )
      :
      theFullID( fullid ),
      thePropertyName( propertyname )
    {
      ; // do nothing
    }

    FullPN( FullPNCref fullpn )
      :
      theFullID( fullpn.getFullID() ),
      thePropertyName( fullpn.getPropertyName() )
    {
      ; // do nothing
    }

    FullPN( StringCref fullpropertynamestring );

    ~FullPN() 
    {
      ; // do nothing
    }


    const FullIDCref     getFullID()        const
    {
      return theFullID;
    }

    const EntityType  getEntityType() const 
    { 
      return getFullID().getEntityType(); 
    }

    const SystemPathCref getSystemPath()    const
    { 
      return getFullID().getSystemPath();
    }

    const StringCref     getID()            const
    { 
      return getFullID().getID();
    }

    const StringCref     getPropertyName()  const
    {
      return thePropertyName;
    }

    void setEntityType( const EntityType type )
    {
      theFullID.setEntityType( type );
    }

    void setSystemPath( SystemPathCref systempath ) 
    {
      theFullID.setSystemPath( systempath );
    }

    void setID( StringCref id ) 
    {
      theFullID.setID( id );
    }

    void setPropertyName( StringCref propertyname )
    {
      thePropertyName = propertyname;
    }

    const String getString() const;

    bool isValid() const;

    bool operator<( FullPNCref rhs ) const
    {
      if( getFullID() != rhs.getFullID() )
	{
	  return getFullID() < rhs.getFullID();
	}

      return getPropertyName() < rhs.getPropertyName();
    }

    bool operator==( FullPNCref rhs ) const
    {
      if( getFullID() != rhs.getFullID() )
	{
	  return false;
	}

      // finally compare the ID strings
      return getPropertyName() == rhs.getPropertyName();
    }

    bool operator!=( FullPNCref rhs ) const
    {
      return ! operator==( rhs );
    }

  private:

    FullID theFullID;
    String thePropertyName;

  };

  /** @} */ // identifier module

} // namespace libecs

#endif // __FULLID_HPP

/*
  Do not modify
  $Author$
  $Revision$
  $Date$
  $Locker$
*/
