/* Hector -- A Simple Climate Model
   Copyright (C) 2014-2015  Battelle Memorial Institute

   Please see the accompanying file LICENSE.md for additional licensing
   information.
*/
/*
 *  csv_outputstream_visitor.cpp
 *  hector
 *
 *  Created by Ben on 1 March 2011.
 *
 */

#include <fstream>

// some boost headers generate warnings under clang; not our problem, ignore
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <boost/lexical_cast.hpp>
#pragma clang diagnostic pop

#include "dummy_model_component.hpp"
#include "forcing_component.hpp"
#include "halocarbon_component.hpp"
#include "temperature_component.hpp"
#include "bc_component.hpp"
#include "oc_component.hpp"
#include "slr_component.hpp"
#include "o3_component.hpp"
#include "oh_component.hpp"
#include "ch4_component.hpp"
#include "n2o_component.hpp"
#include "core.hpp"
#include "unitval.hpp"
#include "h_util.hpp"
#include "simpleNbox.hpp"
#include "csv_outputstream_visitor.hpp"

namespace Hector {

using namespace std;

//------------------------------------------------------------------------------
/*! \brief Constructor
 *  \param filename The file to write the csv output to.
 */
CSVOutputStreamVisitor::CSVOutputStreamVisitor( ostream& outputStream, const bool printHeader )
:csvFile( outputStream )
{
    if( printHeader ) {
        // Print model version header


        // Print table header



    }
    run_name = "";
    current_date = 0;
    datestring = "";
    spinupstring = "";
}

//------------------------------------------------------------------------------
/*! \brief Destructor
 */
CSVOutputStreamVisitor::~CSVOutputStreamVisitor() {
}

//------------------------------------------------------------------------------
// documentation is inherited
bool CSVOutputStreamVisitor::shouldVisit( const bool is, const double date ) {

    current_date = date;
    in_spinup = is;
    datestring = boost::lexical_cast<string>( date );
    spinupstring = boost::lexical_cast<string>( in_spinup );

    // visit all model periods
    return true;
}

//------------------------------------------------------------------------------
/*! \brief Return text that starts every output line
 */
std::string CSVOutputStreamVisitor::linestamp() {
    return( datestring + DELIMITER + run_name + DELIMITER + spinupstring + DELIMITER );
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( Core* c ) {
    run_name = c->getRun_name();
    core = c;
}

// TODO: have to consolidate these macros into the two MESSAGE ones,
// and shift string literals to D_xxxx definitions

// Macro to send a variable with associated unitval units to some output stream
// Takes s (stream), c (component), xname (variable name), x (output variable)
#define STREAM_UNITVAL( s, c, xname, x ) { \



}

// Macro to send a variable with associated unitval units to some output stream
// This uses new sendMessage interface in imodel_component
// Takes s (stream), c (component), xname (variable name), date
#define STREAM_MESSAGE( s, c, xname ) { \
unitval x = c->sendMessage( M_GETDATA, xname ); \



}
// Macro for date-dependent variables
// Takes s (stream), c (component), xname (variable name), date
#define STREAM_MESSAGE_DATE( s, c, xname, date ) { \
unitval x = c->sendMessage( M_GETDATA, xname, message_data( date ) ); \



}


//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( ForcingComponent* c ) {
    if( !core->outputEnabled( c->getComponentName() ) ) return;
    streamsize oldPrecision = csvFile.precision( 4 );

    if(c->currentYear < c->baseyear)
        return;

    ForcingComponent::forcings_t  forcings = c->forcings_ts.get(c->currentYear);

    // Walk through the forcings map, outputting everything
    for( ForcingComponent::forcingsIterator it = forcings.begin(); it != forcings.end(); ++it ) {
        STREAM_UNITVAL( csvFile, c, ( *it ).first, ( *it ).second );
    }

    csvFile.precision( oldPrecision );
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( SimpleNbox* c ) {
    if( !core->outputEnabled( c->getComponentName() ) ) return;

    // Global outputs
    STREAM_MESSAGE( csvFile, c, D_LAND_CFLUX );
    STREAM_MESSAGE( csvFile, c, D_NPP );
    STREAM_MESSAGE( csvFile, c, D_RH );
    STREAM_MESSAGE( csvFile, c, D_ATMOSPHERIC_CO2 );
    STREAM_MESSAGE( csvFile, c, D_ATMOSPHERIC_C );
    STREAM_MESSAGE( csvFile, c, D_ATMOSPHERIC_C_RESIDUAL );
    STREAM_MESSAGE( csvFile, c, D_VEGC );
    STREAM_MESSAGE( csvFile, c, D_DETRITUSC );
    STREAM_MESSAGE( csvFile, c, D_SOILC );
    STREAM_MESSAGE( csvFile, c, D_EARTHC );

    // Biome-specific outputs: <variable>.<biome>
    if( c->veg_c.size() > 1 ) {
        SimpleNbox::unitval_stringmap::const_iterator it;
        for( it = c->veg_c.begin(); it != c->veg_c.end(); it++ ) {
            std::string biome = ( *it ).first;
            STREAM_UNITVAL( csvFile, c, biome+"."+D_NPP, c->npp( biome ) );
            STREAM_UNITVAL( csvFile, c, biome+"."+D_RH, c->rh( biome ) );
            STREAM_UNITVAL( csvFile, c, biome+"."+D_VEGC, c->veg_c[ biome ] );
            STREAM_UNITVAL( csvFile, c, biome+"."+D_DETRITUSC, c->detritus_c[ biome ] );
            STREAM_UNITVAL( csvFile, c, biome+"."+D_SOILC, c->soil_c[ biome ] );
            STREAM_UNITVAL( csvFile, c, biome+"."+D_TEMPFERTD, unitval( c->tempfertd[ biome ], U_UNITLESS ) );
            STREAM_UNITVAL( csvFile, c, biome+"."+D_TEMPFERTS, unitval( c->tempferts[ biome ], U_UNITLESS ) );
        }
    }
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( HalocarbonComponent* c ) {
    // TODO: how to get emissions in the gas specific units?
    if( !core->outputEnabled( c->getComponentName() ) ) return;
    STREAM_MESSAGE( csvFile, c, D_HC_CONCENTRATION );
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( TemperatureComponent* c ) {
    if( !core->outputEnabled( c->getComponentName() ) ) return;
    STREAM_MESSAGE( csvFile, c, D_GLOBAL_TEMP );
    STREAM_MESSAGE( csvFile, c, D_FLUX_MIXED );
    STREAM_MESSAGE( csvFile, c, D_FLUX_INTERIOR )
	STREAM_MESSAGE(csvFile, c, D_HEAT_FLUX );
    }
//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( OceanComponent* c ) {
    if( !core->outputEnabled( c->getComponentName() ) ) return;
    STREAM_MESSAGE( csvFile, c, D_ATM_OCEAN_FLUX_HL );
    STREAM_MESSAGE( csvFile, c, D_ATM_OCEAN_FLUX_LL );
    STREAM_MESSAGE( csvFile, c, D_CARBON_DO );
    STREAM_MESSAGE( csvFile, c, D_CARBON_HL );
    STREAM_MESSAGE( csvFile, c, D_CARBON_IO );
    STREAM_MESSAGE( csvFile, c, D_CARBON_LL );
    STREAM_MESSAGE( csvFile, c, D_DIC_HL );
    STREAM_MESSAGE( csvFile, c, D_DIC_LL );
    STREAM_MESSAGE( csvFile, c, D_HL_DO );
    STREAM_MESSAGE( csvFile, c, D_OCEAN_CFLUX );
    STREAM_MESSAGE( csvFile, c, D_OMEGAAR_HL );
    STREAM_MESSAGE( csvFile, c, D_OMEGAAR_LL );
    STREAM_MESSAGE( csvFile, c, D_OMEGACA_HL );
    STREAM_MESSAGE( csvFile, c, D_OMEGACA_LL );
    STREAM_MESSAGE( csvFile, c, D_PCO2_HL );
    STREAM_MESSAGE( csvFile, c, D_PCO2_LL );
    STREAM_MESSAGE( csvFile, c, D_PH_HL );
    STREAM_MESSAGE( csvFile, c, D_PH_LL );
    STREAM_MESSAGE( csvFile, c, D_TEMP_HL );
    STREAM_MESSAGE( csvFile, c, D_TEMP_LL );
    STREAM_MESSAGE( csvFile, c, D_OCEAN_C );
    STREAM_MESSAGE( csvFile, c, D_CO3_HL );
    STREAM_MESSAGE( csvFile, c, D_CO3_LL );
    STREAM_MESSAGE( csvFile, c, D_TIMESTEPS );
    if( !in_spinup ) {
        STREAM_MESSAGE( csvFile, c, D_REVELLE_HL );
        STREAM_MESSAGE( csvFile, c, D_REVELLE_LL );
    }
}



//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( slrComponent* c ) {
    if( !core->outputEnabled( c->getComponentName() ) ) return;
    if( current_date == max( c->refperiod_high, c->normalize_year ) ) {
        std::string olddatestring = datestring;
        for( int i=core->getStartDate()+1; i<current_date; i++ ) {
            // TODO: this is a hack; need to fool the linestamp routine above
            datestring = boost::lexical_cast<string>( i );      // convert to string and store
            STREAM_MESSAGE_DATE( csvFile, c, D_SL_RC, i );
            STREAM_MESSAGE_DATE( csvFile, c, D_SLR, i );
            STREAM_MESSAGE_DATE( csvFile, c, D_SL_RC_NO_ICE, i );
            STREAM_MESSAGE_DATE( csvFile, c, D_SLR_NO_ICE, i );
        }
        datestring = olddatestring;
    }
    if( current_date >= max( c->refperiod_high, c->normalize_year ) ) {	// output all previous years
        STREAM_MESSAGE_DATE( csvFile, c, D_SL_RC, current_date );
        STREAM_MESSAGE_DATE( csvFile, c, D_SLR, current_date );
        STREAM_MESSAGE_DATE( csvFile, c, D_SL_RC_NO_ICE, current_date );
        STREAM_MESSAGE_DATE( csvFile, c, D_SLR_NO_ICE, current_date );
    }
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( BlackCarbonComponent* c ) {
    if( !core->outputEnabled( c->getComponentName() ) ) return;
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( OrganicCarbonComponent* c ) {
    if( !core->outputEnabled( c->getComponentName() ) ) return;
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( OzoneComponent* c ) {
    if( !core->outputEnabled( c->getComponentName() ) ) return;
    STREAM_MESSAGE_DATE( csvFile, c, D_ATMOSPHERIC_O3, current_date );
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( OHComponent* c ) {
   if( !core->outputEnabled( c->getComponentName() ) ) return;
 STREAM_MESSAGE_DATE( csvFile, c, D_LIFETIME_OH, current_date );
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( CH4Component* c ) {
   if( !core->outputEnabled( c->getComponentName() ) ) return;
 STREAM_MESSAGE_DATE( csvFile, c, D_ATMOSPHERIC_CH4, current_date );
}

//------------------------------------------------------------------------------
// documentation is inherited
void CSVOutputStreamVisitor::visit( N2OComponent* c ) {
   if( !core->outputEnabled( c->getComponentName() ) ) return;
STREAM_MESSAGE_DATE( csvFile, c, D_ATMOSPHERIC_N2O, current_date );
}

}
