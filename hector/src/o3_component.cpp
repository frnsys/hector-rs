/* Hector -- A Simple Climate Model
   Copyright (C) 2014-2015  Battelle Memorial Institute

   Please see the accompanying file LICENSE.md for additional licensing
   information.
*/
/*
 *  ozone_component.cpp
 *  hector
 *
 *  Created by Adria Schwarber on 10/24/2013.
 *
 */

#include <math.h>

#include "o3_component.hpp"
#include "core.hpp"
#include "h_util.hpp"
#include "avisitor.hpp"

namespace Hector {

using namespace std;

//------------------------------------------------------------------------------
/*! \brief Constructor
 */
OzoneComponent::OzoneComponent() {
}

//------------------------------------------------------------------------------
/*! \brief Destructor
 */
OzoneComponent::~OzoneComponent() {
}

//------------------------------------------------------------------------------
// documentation is inherited
string OzoneComponent::getComponentName() const {
    const string name = OZONE_COMPONENT_NAME;

    return name;
}

//------------------------------------------------------------------------------
// documentation is inherited
void OzoneComponent::init( Core* coreptr ) {
    logger.open( getComponentName(), false, coreptr->getGlobalLogger().getEchoToFile(), coreptr->getGlobalLogger().getMinLogLevel() );

    core = coreptr;

    CO_emissions.allowInterp( true );
    NMVOC_emissions.allowInterp( true );
    NOX_emissions.allowInterp( true );//Inputs like CO and NMVOC and NOX,
    O3.allowInterp(true);



	// Inform core what data we can provide
    core->registerCapability(  D_ATMOSPHERIC_O3, getComponentName() );
    // Register the emissions we accept (note that OH component also
    // accepts these.  That's ok
    core->registerInput(D_EMISSIONS_CO, getComponentName());
    core->registerInput(D_EMISSIONS_NMVOC, getComponentName());
    core->registerInput(D_EMISSIONS_NOX, getComponentName());
}

//------------------------------------------------------------------------------
// documentation is inherited
unitval OzoneComponent::sendMessage( const std::string& message,
                                    const std::string& datum,
                                    const message_data info )
{
    unitval returnval;

    if( message==M_GETDATA ) {          //! Caller is requesting data
        return getData( datum, info.date );

    } else if( message==M_SETDATA ) {   //! Caller is requesting to set data
        setData(datum, info);
        //TODO: change core so that parsing is routed through sendMessage
        //TODO: make setData private

    } else {                        //! We don't handle any other messages
        H_THROW( "Caller sent unknown message: "+message );
    }

    return returnval;
}

//------------------------------------------------------------------------------
// documentation is inherited
void OzoneComponent::setData( const string& varName,
                              const message_data& data )
{


    try {
        if (  varName == D_PREINDUSTRIAL_O3 ) {
            H_ASSERT( data.date == Core::undefinedIndex() , "date not allowed" );
            PO3 = data.getUnitval(U_DU_O3);
        } else if( varName == D_EMISSIONS_NOX ) {
            H_ASSERT( data.date != Core::undefinedIndex(), "date required" );
            NOX_emissions.set( data.date, data.getUnitval(U_TG_N));
        } else if( varName == D_EMISSIONS_CO ) {
            H_ASSERT( data.date != Core::undefinedIndex(), "date required" );
            CO_emissions.set( data.date, data.getUnitval(U_TG_CO));
        } else if( varName == D_EMISSIONS_NMVOC ) {
            H_ASSERT( data.date != Core::undefinedIndex(), "date required" );
            NMVOC_emissions.set( data.date, data.getUnitval(U_TG_NMVOC));
        } else {
            H_THROW( "Unknown variable name while parsing " + getComponentName() + ": "
                    + varName );
        }
    } catch( h_exception& parseException ) {
        H_RETHROW( parseException, "Could not parse var: "+varName );
    }
}

//------------------------------------------------------------------------------
// documentation is inherited
void OzoneComponent::prepareToRun() {


    oldDate = core->getStartDate();
    O3.set(oldDate, PO3);  // set the first year's value
}

//------------------------------------------------------------------------------
// documentation is inherited
void OzoneComponent::run( const double runToDate ) {

	// Calculate O3 based on NOX, CO, NMVOC, CH4.
    // Modified from Tanaka et al 2007

    unitval current_nox = NOX_emissions.get( runToDate );
	unitval current_co = CO_emissions.get( runToDate );
	unitval current_nmvoc = NMVOC_emissions.get( runToDate );
	unitval current_ch4 = core->sendMessage( M_GETDATA, D_ATMOSPHERIC_CH4, runToDate );

    O3.set( runToDate, unitval( ( 5*log( current_ch4 ) ) + ( 0.125*current_nox ) + ( 0.0011*current_co )
               + ( 0.0033*current_nmvoc ), U_DU_O3 ) );

    oldDate=runToDate;


}

//------------------------------------------------------------------------------
// documentation is inherited
unitval OzoneComponent::getData( const std::string& varName,
                                 const double date ) {

    unitval returnval;

    if( varName == D_ATMOSPHERIC_O3 ) {
        H_ASSERT( date != Core::undefinedIndex(), "Date required for O3" ); //== is a comparision
		returnval = O3.get( date );
    } else {
        H_THROW( "Caller is requesting unknown variable: " + varName );
    }

    return returnval;
}


void OzoneComponent::reset(double time)
{
    O3.truncate(time);
    oldDate = time;


}

//------------------------------------------------------------------------------
// documentation is inherited
void OzoneComponent::shutDown() {

    logger.close();
}

//------------------------------------------------------------------------------
// documentation is inherited
void OzoneComponent::accept( AVisitor* visitor ) {
    visitor->visit( this );
}

}
