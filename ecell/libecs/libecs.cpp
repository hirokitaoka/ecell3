//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//       This file is part of the E-Cell System
//
//       Copyright (C) 1996-2008 Keio University
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
// written by Koichi Takahashi <shafi@e-cell.org>,
// E-Cell Project.
//
#ifdef HAVE_CONFIG_H
#include "ecell_config.h"
#endif /* HAVE_CONFIG_H */

#if defined( WIN32 ) && !defined( __CYGWIN__ )
#include "win32_utils.h"
#endif /* WIN32 */

#ifdef DLL_EXPORT
#undef DLL_EXPORT
#include <gsl/gsl_rng.h>
#define DLL_EXPORT
#else
#include <gsl/gsl_rng.h>
#endif

#include <boost/shared_ptr.hpp>

#include "libecs.hpp"
#include "PropertiedObjectMaker.hpp"
#include "ModuleManager.hpp"
#include "StaticDynamicModule.hpp"
#include "System.hpp"
#include "Variable.hpp"
#include "DiscreteEventStepper.hpp"
#include "DiscreteTimeStepper.hpp"
#include "PassiveStepper.hpp"

namespace libecs
{
int const MAJOR_VERSION( ECELL_MAJOR_VERSION );
int const MINOR_VERSION( ECELL_MINOR_VERSION );
int const MICRO_VERSION( ECELL_MICRO_VERSION );

char const* const VERSION_STRING( ECELL_VERSION_STRING );

static volatile bool isInitialized = false;

static ModuleManager moduleManager;

static PropertiedObjectMaker propertiedObjectMaker( &moduleManager );

const PropertiedObjectMaker& getPropertiedObjectMaker()
{
    return propertiedObjectMaker;
}

bool initialize()
{
    /* XXX: not atomic - "compare and swap" needed for concurrency */
    if ( isInitialized )
        return true;
    else
        isInitialized = true;

    if ( ModuleMaker::initialize() )
    {
        return false;
    }
#if defined( WIN32 ) && !defined( __CYGWIN__ )
    if ( libecs_win32_init() )
    {
        ModuleMaker::finalize();
        return false;
    }
#endif

    gsl_rng_env_setup();

    moduleManager.addModuleMaker(
            boost::shared_ptr< ModuleManager::ModuleMaker >(
                reinterpret_cast< ModuleManager::ModuleMaker* >(
                        new SharedModuleMaker< PropertiedClass >() ) ) );

    StaticModuleMaker< PropertiedClass >* builtinModuleMaker = 
            reinterpret_cast< StaticModuleMaker< PropertiedClass >* >(
                new StaticModuleMaker< PropertiedClass >() );

    builtinModuleMaker->addClass(
            reinterpret_cast< StaticDynamicModule< PropertiedClass >* >(
                new StaticDynamicModule< System >() ) );

    builtinModuleMaker->addClass(
            reinterpret_cast< StaticDynamicModule< PropertiedClass >* >(
                new StaticDynamicModule< Variable >() ) );

    builtinModuleMaker->addClass(
            reinterpret_cast< StaticDynamicModule< PropertiedClass >* >(
                new StaticDynamicModule< DiscreteEventStepper >() ) );

    builtinModuleMaker->addClass(
            reinterpret_cast< StaticDynamicModule< PropertiedClass >* >(
                new StaticDynamicModule< DiscreteTimeStepper >() ) );

    builtinModuleMaker->addClass(
            reinterpret_cast< StaticDynamicModule< PropertiedClass >* >(
                new StaticDynamicModule< PassiveStepper >() ) );

    moduleManager.addModuleMaker(
            boost::shared_ptr< ModuleManager::ModuleMaker >(
                builtinModuleMaker ) );

    return true;
}

void finalize()
{
    /* XXX: not atomic - "compare and swap" needed for concurrency */
    if ( !isInitialized )
        return;
    else
        isInitialized = false;

#if defined( WIN32 ) && !defined( __CYGWIN__ )
    libecs_win32_fini();
#endif
    ModuleMaker::finalize();
}

void setDMSearchPath( const std::string& path )
{
    ModuleMaker::setSearchPath( path );
}

const std::string getDMSearchPath()
{
    return ModuleMaker::getSearchPath();
}

const int getMajorVersion()
{
    return MAJOR_VERSION;
}

const int getMinorVersion()
{
    return MINOR_VERSION;
}

const int getMicroVersion()
{
    return MICRO_VERSION;
}

const std::string getVersion()
{
    return VERSION_STRING;
}

} // namespace libecs
