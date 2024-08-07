/* Hector -- A Simple Climate Model
   Copyright (C) 2014-2015  Battelle Memorial Institute

   Please see the accompanying file LICENSE.md for additional licensing
   information.
*/
/*
 *  forcing_component.cpp
 *  hector
 *
 *  Created by Ben on 02 March 2011.
 *
 */

// some boost headers generate warnings under clang; not our problem, ignore
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <boost/array.hpp>
#pragma clang diagnostic pop

#include "forcing_component.hpp"
#include "avisitor.hpp"

namespace Hector {

/* These next two arrays and the map that connects them are a
 * workaround for the problems created by storing the halocarbon
 * forcings in the halocarbon components.  Because the halocarbon
 * components don't know about the base year adjustments, they can't
 * provide the forcings relative to the base year, which is what
 * outside callers will generally want.  Internally, however, we still
 * need to be able to get the raw forcings from the halocarbon
 * components, so we can't just change everything to point at the
 * forcing component (which would return the base year adjusted value).
 *
 * The solution we adopted was to create a second set of capabilities
 * to refer to the adjusted values, and we let the forcing component
 * intercept those.  However, the forcing values themselves are stored
 * under the names used for the unadjusted values, so we have to have
 * a name translation table so that we can find the data that the
 * message is asking for.  In the end, the whole process winds up
 * being a little ugly, but it gets the job done.
 */

const char *ForcingComponent::adjusted_halo_forcings[N_HALO_FORCINGS] = {
    D_RFADJ_CF4,
    D_RFADJ_C2F6,
    D_RFADJ_HFC23,
    D_RFADJ_HFC32,
    D_RFADJ_HFC4310,
    D_RFADJ_HFC125,
    D_RFADJ_HFC134a,
    D_RFADJ_HFC143a,
    D_RFADJ_HFC227ea,
    D_RFADJ_HFC245fa,
    D_RFADJ_SF6,
    D_RFADJ_CFC11,
    D_RFADJ_CFC12,
    D_RFADJ_CFC113,
    D_RFADJ_CFC114,
    D_RFADJ_CFC115,
    D_RFADJ_CCl4,
    D_RFADJ_CH3CCl3,
    D_RFADJ_HCFC22,
    D_RFADJ_HCFC141b,
    D_RFADJ_HCFC142b,
    D_RFADJ_halon1211,
    D_RFADJ_halon1301,
    D_RFADJ_halon2402,
    D_RFADJ_CH3Cl,
    D_RFADJ_CH3Br
};

const char *ForcingComponent::halo_forcing_names[N_HALO_FORCINGS] = {
    D_RF_CF4,
    D_RF_C2F6,
    D_RF_HFC23,
    D_RF_HFC32,
    D_RF_HFC4310,
    D_RF_HFC125,
    D_RF_HFC134a,
    D_RF_HFC143a,
    D_RF_HFC227ea,
    D_RF_HFC245fa,
    D_RF_SF6,
    D_RF_CFC11,
    D_RF_CFC12,
    D_RF_CFC113,
    D_RF_CFC114,
    D_RF_CFC115,
    D_RF_CCl4,
    D_RF_CH3CCl3,
    D_RF_HCFC22,
    D_RF_HCFC141b,
    D_RF_HCFC142b,
    D_RF_halon1211,
    D_RF_halon1301,
    D_RF_halon2402,
    D_RF_CH3Cl,
    D_RF_CH3Br
};

std::map<std::string, std::string> ForcingComponent::forcing_name_map;

using namespace std;

//------------------------------------------------------------------------------
/*! \brief Constructor
 */
ForcingComponent::ForcingComponent() {
}

//------------------------------------------------------------------------------
/*! \brief Destructor
 */
ForcingComponent::~ForcingComponent() {
}

//------------------------------------------------------------------------------
// documentation is inherited
string ForcingComponent::getComponentName() const {
    const string name = FORCING_COMPONENT_NAME;
    return name;
}

//------------------------------------------------------------------------------
// documentation is inherited
void ForcingComponent::init( Core* coreptr ) {

    logger.open( getComponentName(), false, coreptr->getGlobalLogger().getEchoToFile(), coreptr->getGlobalLogger().getMinLogLevel() );


    core = coreptr;

    baseyear = 0.0;
    currentYear = 0.0;

    Ftot_constrain.allowInterp( true );
    Ftot_constrain.name = D_RF_TOTAL;

    // Register the data we can provide
    core->registerCapability( D_RF_TOTAL, getComponentName() );
    core->registerCapability( D_RF_BASEYEAR, getComponentName() );
    core->registerCapability( D_RF_CO2, getComponentName());
    core->registerCapability( D_RF_CH4, getComponentName());
    core->registerCapability( D_RF_N2O, getComponentName());
    core->registerCapability( D_RF_H2O_STRAT, getComponentName());
    core->registerCapability( D_RF_O3_TROP, getComponentName());
    core->registerCapability( D_RF_BC, getComponentName());
    core->registerCapability( D_RF_OC, getComponentName());
    core->registerCapability( D_RF_SO2d, getComponentName());
    core->registerCapability( D_RF_SO2i, getComponentName());
    core->registerCapability( D_RF_SO2, getComponentName());
    core->registerCapability( D_RF_VOL, getComponentName());
    for(int i=0; i<N_HALO_FORCINGS; ++i) {
        core->registerCapability(adjusted_halo_forcings[i], getComponentName());
        forcing_name_map[adjusted_halo_forcings[i]] = halo_forcing_names[i];
    }

    // Register our dependencies

    core->registerDependency( D_ATMOSPHERIC_CH4, getComponentName() );
    core->registerDependency( D_ATMOSPHERIC_CO2, getComponentName() );
    core->registerDependency( D_ATMOSPHERIC_O3, getComponentName() );
    core->registerDependency( D_EMISSIONS_BC, getComponentName() );
    core->registerDependency( D_EMISSIONS_OC, getComponentName() );
    core->registerDependency( D_NATURAL_SO2, getComponentName() );
    core->registerDependency( D_ATMOSPHERIC_N2O, getComponentName() );

    core->registerDependency( D_RF_CF4, getComponentName() );
    core->registerDependency( D_RF_C2F6, getComponentName() );
    core->registerDependency( D_RF_HFC23, getComponentName() );
    core->registerDependency( D_RF_HFC32, getComponentName() );
    core->registerDependency( D_RF_HFC4310, getComponentName() );
    core->registerDependency( D_RF_HFC125, getComponentName() );
    core->registerDependency( D_RF_HFC134a, getComponentName() );
    core->registerDependency( D_RF_HFC143a, getComponentName() );
    core->registerDependency( D_RF_HFC227ea, getComponentName() );
    core->registerDependency( D_RF_HFC245fa, getComponentName() );
    core->registerDependency( D_RF_SF6, getComponentName() );
    core->registerDependency( D_RF_CFC11, getComponentName() );
    core->registerDependency( D_RF_CFC12, getComponentName() );
    core->registerDependency( D_RF_CFC113, getComponentName() );
    core->registerDependency( D_RF_CFC114, getComponentName() );
    core->registerDependency( D_RF_CFC115, getComponentName() );
    core->registerDependency( D_RF_CCl4, getComponentName() );
    core->registerDependency( D_RF_CH3CCl3, getComponentName() );
    core->registerDependency( D_RF_HCFC22, getComponentName() );
    core->registerDependency( D_RF_HCFC141b, getComponentName() );
    core->registerDependency( D_RF_HCFC142b, getComponentName() );
    core->registerDependency( D_RF_halon1211, getComponentName() );
    core->registerDependency( D_RF_halon1301, getComponentName() );
    core->registerDependency( D_RF_halon2402, getComponentName() );
    core->registerDependency( D_RF_CH3Br, getComponentName() );
    core->registerDependency( D_RF_CH3Cl, getComponentName() );
    core->registerDependency( D_RF_T_ALBEDO, getComponentName() );
}

//------------------------------------------------------------------------------
// documentation is inherited
unitval ForcingComponent::sendMessage( const std::string& message,
                                      const std::string& datum,
                                      const message_data info )
{
    unitval returnval;

    if( message==M_GETDATA ) {          //! Caller is requesting data
        return getData( datum, info.date );

    } else if( message==M_SETDATA ) {   //! Caller is requesting to set data
        //TODO: call setData below
        //TODO: change core so that parsing is routed through sendMessage
        //TODO: make setData private

    } else {                        //! We don't handle any other messages
        H_THROW( "Caller sent unknown message: "+message );
    }

    return returnval;
}

//------------------------------------------------------------------------------
// documentation is inherited
void ForcingComponent::setData( const string& varName,
                                const message_data& data )
{


    try {
        if( varName == D_RF_BASEYEAR ) {
            H_ASSERT( data.date == Core::undefinedIndex(), "date not allowed" );
            baseyear = data.getUnitval(U_UNDEFINED);
        } else if( varName == D_FTOT_CONSTRAIN ) {
            H_ASSERT( data.date != Core::undefinedIndex(), "date required" );
            Ftot_constrain.set(data.date, data.getUnitval(U_W_M2));
        } else {

            H_THROW( "Unknown variable name while parsing "+ getComponentName() + ": "
                    + varName );
        }
    } catch( h_exception& parseException ) {
        H_RETHROW( parseException, "Could not parse var: "+varName );
    }
}

//------------------------------------------------------------------------------
// documentation is inherited
void ForcingComponent::prepareToRun() {



    if( baseyear==0.0 )
        baseyear = core->getStartDate() + 1;        // default, if not supplied by user


    H_ASSERT( baseyear > core->getStartDate(), "Base year must be >= model start date" );

    if( Ftot_constrain.size() ) {
        Logger& glog = core->getGlobalLogger();

    }

    baseyear_forcings.clear();
}

//------------------------------------------------------------------------------
// documentation is inherited
void ForcingComponent::run( const double runToDate ) {

    // Calculate instantaneous radiative forcing for any & all agents
    // As each is computed, push it into 'forcings' map for Ftot calculation.
	// Note that forcings have to be mutually exclusive, there are no subtotals for different species.

    currentYear = runToDate;

    if( runToDate < baseyear ) {

    } else {
        forcings_t forcings;

        // ---------- CO2 ----------
        // Instantaneous radiative forcings for CO2, CH4, and N2O from http://www.esrl.noaa.gov/gmd/aggi/
        // These are in turn from IPCC (2001)

        // This is identical to that of MAGICC; see Meinshausen et al. (2011)
        unitval Ca = core->sendMessage( M_GETDATA, D_ATMOSPHERIC_CO2 );
        if( runToDate==baseyear )
            C0 = Ca;
        forcings[D_RF_CO2 ].set( 5.35 * log( Ca/C0 ), U_W_M2 );

        // ---------- Terrestrial albedo ----------
        if( core->checkCapability( D_RF_T_ALBEDO ) ) {
            forcings[ D_RF_T_ALBEDO ] = core->sendMessage( M_GETDATA, D_RF_T_ALBEDO, message_data( runToDate ) );
        }

        // ---------- N2O and CH4 ----------
        // Equations from Joos et al., 2001
        if( core->checkCapability( D_ATMOSPHERIC_CH4 ) && core->checkCapability( D_ATMOSPHERIC_N2O ) ) {

#define f(M,N) 0.47 * log( 1 + 2.01 * 1e-5 * pow( M * N, 0.75 ) + 5.31 * 1e-15 * M * pow( M * N, 1.52 ) )
            double Ma = core->sendMessage( M_GETDATA, D_ATMOSPHERIC_CH4, message_data( runToDate ) ).value( U_PPBV_CH4 );
            double M0 = core->sendMessage( M_GETDATA, D_PREINDUSTRIAL_CH4 ).value( U_PPBV_CH4 );
            double Na = core->sendMessage( M_GETDATA, D_ATMOSPHERIC_N2O, message_data( runToDate ) ).value( U_PPBV_N2O );
            double N0 = core->sendMessage( M_GETDATA, D_PREINDUSTRIAL_N2O ).value( U_PPBV_N2O );

            double fch4 =  0.036 * ( sqrt( Ma ) - sqrt( M0 ) ) - ( f( Ma, N0 ) - f( M0, N0 ) );
            forcings[D_RF_CH4].set( fch4, U_W_M2 );

            double fn2o =  0.12 * ( sqrt( Na ) - sqrt( N0 ) ) - ( f( M0, Na ) - f( M0, N0 ) );
            forcings[D_RF_N2O].set( fn2o, U_W_M2 );

            // ---------- Stratospheric H2O from CH4 oxidation ----------
            // From Tanaka et al, 2007, but using Joos et al., 2001 value of 0.05
            const double fh2o = 0.05 * ( 0.036 * ( sqrt( Ma ) - sqrt( M0 ) ) );
            forcings[D_RF_H2O_STRAT].set( fh2o, U_W_M2 );
        }

        // ---------- Troposheric Ozone ----------
        if( core->checkCapability( D_ATMOSPHERIC_O3 ) ) {
            //from Tanaka et al, 2007
            const double ozone = core->sendMessage( M_GETDATA, D_ATMOSPHERIC_O3, message_data( runToDate ) ).value( U_DU_O3 );
            const double fo3 = 0.042 * ozone;
            forcings[D_RF_O3_TROP].set( fo3, U_W_M2 );
        }

        // ---------- Halocarbons ----------
        // TODO: Would like to just 'know' all the halocarbon instances out there
        boost::array<string, 26> halos = {
            {
                D_RF_CF4,
                D_RF_C2F6,
                D_RF_HFC23,
                D_RF_HFC32,
                D_RF_HFC4310,
                D_RF_HFC125,
                D_RF_HFC134a,
                D_RF_HFC143a,
                D_RF_HFC227ea,
                D_RF_HFC245fa,
                D_RF_SF6,
                D_RF_CFC11,
                D_RF_CFC12,
                D_RF_CFC113,
                D_RF_CFC114,
                D_RF_CFC115,
                D_RF_CCl4,
                D_RF_CH3CCl3,
                D_RF_HCFC22,
                D_RF_HCFC141b,
                D_RF_HCFC142b,
                D_RF_halon1211,
                D_RF_halon1301,
                D_RF_halon2402,
                D_RF_CH3Cl,
                D_RF_CH3Br
            }
        };

        // Halocarbons can be disabled individually via the input file, so we run through all possible ones
         for (unsigned hc=0; hc<halos.size(); ++hc) {
            if( core->checkCapability( halos[hc] ) ) {
                // Forcing values are actually computed by the halocarbon itself
                forcings[ halos[hc] ] = core->sendMessage( M_GETDATA, halos[hc], message_data( runToDate ) );
                }
        }

        // ---------- Black carbon ----------
        if( core->checkCapability( D_EMISSIONS_BC ) ) {
            double fbc = 0.0743 * core->sendMessage( M_GETDATA, D_EMISSIONS_BC, message_data( runToDate ) ).value( U_TG );
            forcings[D_RF_BC].set( fbc, U_W_M2 );
            // includes both indirect and direct forcings from Bond et al 2013, Journal of Geophysical Research Atmo (table C1 - Central)
        }

        // ---------- Organic carbon ----------
        if( core->checkCapability( D_EMISSIONS_OC ) ) {
            double foc = -0.0128 * core->sendMessage( M_GETDATA, D_EMISSIONS_OC, message_data( runToDate ) ).value( U_TG );
            forcings[D_RF_OC].set( foc, U_W_M2 );
            // includes both indirect and direct forcings from Bond et al 2013, Journal of Geophysical Research Atmo (table C1 - Central).
            // The fossil fuel and biomass are weighted (-4.5) then added to the snow and clouds for a total of -12.8 (personal communication Steve Smith, PNNL)
        }

        // ---------- Sulphate Aerosols ----------
        if( core->checkCapability( D_NATURAL_SO2 ) && core->checkCapability( D_EMISSIONS_SO2 ) ) {

            unitval S0 = core->sendMessage( M_GETDATA, D_2000_SO2 );
            unitval SN = core->sendMessage( M_GETDATA, D_NATURAL_SO2 );

            // Includes only direct forcings from Forster et al 2007 (IPCC)
            // Equations from Joos et al., 2001
            H_ASSERT( S0.value( U_GG_S ) >0, "S0 is 0" );
            unitval emission = core->sendMessage( M_GETDATA, D_EMISSIONS_SO2, message_data( runToDate ) );
            double fso2d = -0.35 * emission/S0;
            forcings[D_RF_SO2d].set( fso2d, U_W_M2 );
            // includes only direct forcings from Forster etal 2007 (IPCC)

            // Indirect aerosol effect via changes in cloud properties
            const double a = -0.6 * ( log( ( SN.value( U_GG_S ) + emission.value( U_GG_S ) ) / SN.value( U_GG_S ) ) ); // -.6
            const double b =  pow ( log ( ( SN.value( U_GG_S ) + S0.value( U_GG_S ) ) / SN.value( U_GG_S ) ), -1 );
            double fso2i = a * b;
            forcings[D_RF_SO2i].set( fso2i, U_W_M2 );
        }

        if( core->checkCapability( D_VOLCANIC_SO2 ) ) {
            // Volcanic forcings
            forcings[D_RF_VOL] = core->sendMessage( M_GETDATA, D_VOLCANIC_SO2, message_data( runToDate ) );
        }

        // ---------- Total ----------
        unitval Ftot( 0.0, U_W_M2 );  // W/m2
        for( forcingsIterator it = forcings.begin(); it != forcings.end(); ++it ) {
            Ftot = Ftot + ( *it ).second;

        }

        // If the user has supplied total forcing data, use that
        if( Ftot_constrain.size() && runToDate <= Ftot_constrain.lastdate() ) {

            forcings[ D_RF_TOTAL ] = Ftot_constrain.get( runToDate );
        } else {
            forcings[ D_RF_TOTAL ] = Ftot;
        }


        //---------- Change to relative forcing ----------
        // Note that the code below assumes model is always consistently run from base-year forward.
        // Results will not be consistent if parameters are changed but base-year is not re-run.

       // At this point, we've computed all absolute forcings. If base year, save those values
        if( runToDate==baseyear ) {

            baseyear_forcings = forcings;
        }

        // Subtract base year forcing values from forcings, i.e. make them relative to base year
        for( forcingsIterator it = forcings.begin(); it != forcings.end(); ++it ) {
            forcings[ ( *it ).first ] = ( *it ).second - baseyear_forcings[ ( *it ).first ];
        }

        // Store the forcings that we have calculated
        forcings_ts.set(runToDate, forcings);
    }
}

//------------------------------------------------------------------------------
// documentation is inherited
unitval ForcingComponent::getData( const std::string& varName,
                                  const double date ) {


    unitval returnval;
    double getdate = date;             // This is why I hate declaring PBV args as const!

    if(getdate == Core::undefinedIndex()) {
        // If no date specified, provide the current date
        getdate = currentYear;
    }

    if(getdate < baseyear) {
        // Forcing component hasn't run yet, so there is no data to get.
        returnval.set(0.0, U_W_M2);
        return returnval;
    }







    forcings_t forcings(forcings_ts.get(getdate));

    if( varName == D_RF_BASEYEAR ) {
        returnval.set( baseyear, U_UNITLESS );
    } else if (varName == D_RF_SO2) {
        // total SO2 forcing
        std::map<std::string, unitval>::const_iterator forcing_SO2d = forcings.find( D_RF_SO2d );
        std::map<std::string, unitval>::const_iterator forcing_SO2i = forcings.find( D_RF_SO2i );
        if ( forcing_SO2d != forcings.end() ) {
            if ( forcing_SO2i != forcings.end() ) {
                returnval = forcing_SO2d->second + forcing_SO2i->second;
            } else {
                returnval = forcing_SO2d->second;
            }
        } else {
            if ( forcing_SO2i != forcings.end() ) {
                returnval = forcing_SO2i->second;
            } else {
                returnval.set( 0.0, U_W_M2 );
            }
        }
    } else {
        std::string forcing_name;
        auto forcit = forcing_name_map.find(varName);
        if(forcit != forcing_name_map.end()) {
            forcing_name = forcing_name_map[varName];
        }
        else {
            forcing_name = varName;
        }
        std::map<std::string, unitval>::const_iterator forcing = forcings.find(forcing_name);
        if ( forcing != forcings.end() ) {
            // from the forcing map
            returnval = forcing->second;
        } else {
            if (currentYear < baseyear) {
                returnval.set( 0.0, U_W_M2 );
            } else {
                H_THROW( "Caller is requesting unknown variable: " + varName );
            }
        }
    }

    return returnval;
}

void ForcingComponent::reset(double time)
{
    // Set the current year to the reset year, and drop outputs after the reset year.
    currentYear = time;
    forcings_ts.truncate(time);


}


//------------------------------------------------------------------------------
// documentation is inherited
void ForcingComponent::shutDown() {

    logger.close();
}

//------------------------------------------------------------------------------
// documentation is inherited
void ForcingComponent::accept( AVisitor* visitor ) {
    visitor->visit( this );
}

}
