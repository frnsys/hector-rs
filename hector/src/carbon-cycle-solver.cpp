/* Hector -- A Simple Climate Model
   Copyright (C) 2014-2015  Battelle Memorial Institute

   Please see the accompanying file LICENSE.md for additional licensing
   information.
*/
 /*
 * carbon-cycle-solver.cpp
 *
 * ODE solver for integrating the carbon cycle in hector.
 *
 * See notes in carbon-cycle-solver.cpp
 *
 */

#include <math.h>
#include <string>

// some boost headers generate warnings under clang; not our problem, ignore
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <boost/numeric/odeint.hpp>
#pragma clang diagnostic pop

#include "carbon-cycle-solver.hpp"
#include "avisitor.hpp"

namespace Hector {

//------------------------------------------------------------------------------
/*! \brief Constructor
 */
CarbonCycleSolver::CarbonCycleSolver() : nc( 0 ),
eps_abs( 1.0e-6 ),eps_rel( 1.0e-6 ),
dt( 0.3 )
{
}

//------------------------------------------------------------------------------
/*! \brief Deconstructor
 */
CarbonCycleSolver::~CarbonCycleSolver()
{
}

//------------------------------------------------------------------------------
// documentation is inherited
void CarbonCycleSolver::init( Core* coreptr ) {
    // This component is very verbose at the debug and notice levels, so limit
    // output to the warning level, even if the rest of the model is configured
    // for something lower.
    logger.open(getComponentName(), false, coreptr->getGlobalLogger().getEchoToFile(), Logger::WARNING);


    core = coreptr;

    in_spinup = false;

    // We want to run after the carbon box models, to give them a chance to initialize
    core->registerDependency( D_ATMOSPHERIC_C, getComponentName() );
}

//------------------------------------------------------------------------------
// documentation is inherited
unitval CarbonCycleSolver::sendMessage( const std::string& message,
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
void CarbonCycleSolver::setData( const std::string &varName,
                                 const message_data& data )
{


    try {
        if( varName == D_CCS_EPS_ABS ) {
            H_ASSERT( data.date == Core::undefinedIndex() , "date not allowed" );
            eps_abs = data.getUnitval(U_UNDEFINED);;
        }
        else if( varName == D_CCS_EPS_REL ) {
            H_ASSERT( data.date == Core::undefinedIndex() , "date not allowed" );
            eps_rel = data.getUnitval(U_UNDEFINED);;
        }
        else if( varName == D_CCS_DT ) {
            H_ASSERT( data.date == Core::undefinedIndex() , "date not allowed" );
            dt = data.getUnitval(U_UNDEFINED);
        }
        else if( varName == D_EPS_SPINUP ) {
            H_ASSERT( data.date == Core::undefinedIndex() , "date not allowed" );
            eps_spinup = data.getUnitval(U_PGC);
        }
        else {

            H_THROW( "Unknown variable name while parsing "+ getComponentName() + ": "
                    + varName );
        }
    } catch( h_exception& parseException ) {
        H_RETHROW( parseException, "Could not parse var: "+varName );
    }

    return;
}

//------------------------------------------------------------------------------
// documentation is inherited
void CarbonCycleSolver::prepareToRun()
{


    cmodel = dynamic_cast<CarbonCycleModel*>( core->getComponentByCapability( D_ATMOSPHERIC_C ) );

    // initialize the solver's internal data
    t = core->getStartDate();
    nc = cmodel->ncpool();


    H_ASSERT( nc > 0, "nc must be > 0" );
    // resize the array of carbon pool values
    c.resize(nc);

}

//------------------------------------------------------------------------------
// documentation is inherited
unitval CarbonCycleSolver::getData( const std::string& varName,
                                   const double date ) {

    unitval returnval;

    H_ASSERT( date == Core::undefinedIndex(), "Date not allowed for CarbonCycleSolver" );

    H_THROW( "Caller is requesting unknown variable: " + varName );

    return returnval;
}

void CarbonCycleSolver::reset(double time)
{
    // Only state maintained by this component is the time counter
    t = time;
    in_spinup = false;          // reset this in case we will be expected to rerun the spinup.


}



//------------------------------------------------------------------------------
// documentation is inherited
void CarbonCycleSolver::shutDown()
{

    logger.close();
}


//------------------------------------------------------------------------------
/*! \brief              Dispatch function called by ODE solver
 *  \param[in] y        pools
 *  \param[in] dydt     pool changes
 *  \param[in] t        time
 *  \exception          If the carbon model returned failure flag we must throw
 *                      an exception to stop the ODE solver.
 */
void CarbonCycleSolver::ODEEvalFunctor::operator()( const std::vector<double>& y,
                                                    std::vector<double>& dydt,
                                                    double t )
{
    // Note the std garuntees vetors are contigous so we can convert to array by
    // taking the address of the first value.
    int status = modelptr->calcderivs( t, &y[0], &dydt[0] );

    if( status != ODE_SUCCESS ) {
        bad_derivative_exception e(status);
        throw e;
    }
}

//------------------------------------------------------------------------------
/*! \brief              Observer callback called by ODE solver when a successful
 *                      step has been taken.
 *  \details            We use this callback to update our state variable time (t)
 *                      and in principle carbon pools (c) however the later is
 *                      already updated by the solver (pass by reference) so we will
 *                      skip copying here.
 *  \param[in] y        pools
 *  \param[in] t        time
 */
void CarbonCycleSolver::ODEEvalFunctor::operator()( const std::vector<double>& y, double t ) {
    // copy the current time to the original in CarbonCycleSolver
    (*this->t) = t;
}

//------------------------------------------------------------------------------
/*! \brief Support function for gsl_ode failure
 *  \param[in] stat     failure code
 *  \param[in] t0       start of time step
 *  \param[in] tmid     middle of time step
 *  \exception          will always happen; gsl solver has failed
 */
void CarbonCycleSolver::failure( int stat, double t0, double tmid ) {



    for( int i=0; i<nc; ++i )


    H_THROW( "gsl_ode_evolve_apply failed." );
}

//------------------------------------------------------------------------------
// documentation is inherited
void CarbonCycleSolver::run( const double tnew )
{
    if(tnew <= t) {

    }
    H_ASSERT( tnew > t, "solver tnew is not greater than t" );

    // Get the initial state data from the box model. c will be filled in
    // Note that we rely on the box model to handle the units.  Inside the
    // solver we strip the unit values and work with raw numbers.
    cmodel->getCValues( t, &c[0] );

    double t0   = t;  // stash this in case we need to report & diagnose an error
    // Now integrate from the beginning of the time step using the updated
    // slow params.  Note we can discard t0 and the values in cc
    cmodel->slowparameval( t, &c[0] );
    int retry = 0;


    while( t < tnew && retry < MAX_CARBON_MODEL_RETRIES ) {


        double t_start = t;
        double t_target = tnew;

        while( t < t_target && retry < MAX_CARBON_MODEL_RETRIES ) {


            int stat = ODE_SUCCESS;
            ODEEvalFunctor odeFunctor( cmodel, &t );
            try {
                using namespace boost::numeric::odeint;
                typedef runge_kutta_dopri5<std::vector<double> > error_stepper_type;
                integrate_adaptive( make_controlled<error_stepper_type>( eps_abs, eps_rel ),
                         odeFunctor, c, t_start, t_target, dt, odeFunctor );
            } catch( bad_derivative_exception& e ) {
                stat = e.errorFlag;
            }

            if( stat == CARBON_CYCLE_RETRY ) {

                t_target = t_start + ( t_target - t_start ) / 2.0;
                t = t_start;

                dt = t_target - t;
                cmodel->getCValues( t, &c[0] );     // reset pools and inform model of new starting point

            } else if( stat != ODE_SUCCESS )
                failure( stat, t_start, t_target );
        }

        // We have exited GSL solver loop, but did we make it?
        if( retry < MAX_CARBON_MODEL_RETRIES ) {


            retry = 0;
            cmodel->stashCValues( t, &c[0] );   // update state
        } else {


        }
    }
    H_ASSERT( t == tnew, "solver failure: t != tnew" );





    cmodel->record_state(tnew);


}

//------------------------------------------------------------------------------
/*! \brief      Run the carbon cycle to steady state, if possible
 *  \exception  epsilon must be >0
 *
 *  We normally want the carbon model to be in steady state, i.e. with unchanging
 *  carbon pools, at the end of the preindustrial period before the main run.
 *  To reach this point, run the carbon model until all dc/dt are ~0.
 */
bool CarbonCycleSolver::run_spinup( const int step )
{

    if( !in_spinup ) {  // first time
        in_spinup = true;
        t = step-1;
        c_original.resize(nc);
        c_old.resize(nc);
        c_new.resize(nc);
        dcdt.resize(nc);
        // initialize to zero
        for(int i=0; i<nc; ++i)
          c_original[i] = c_old[i] = c_new[i] = dcdt[i] = 0.0;

        cmodel->getCValues( t, &c_original[0] );
        cmodel->record_state(t);
    }

    cmodel->getCValues( t, &c_old[0] );
    run( step );
    cmodel->getCValues( step, &c_new[0] );

    int max_dcdt_pool = 0;
    double max_dcdt = 0.0;

    for( int i=0; i<nc; i++ ) {     // find the biggest difference
        dcdt[ i ] = fabs( c_new[ i ] - c_old[ i ] );
        c_old[ i ] = c_new[ i ];
        if (dcdt[ i ] > max_dcdt) {
            max_dcdt = dcdt[i];
        }
    }

    bool spunup = ( max_dcdt < eps_spinup.value( U_PGC ) );

    if( spunup ) {
        Logger& glog = core->getGlobalLogger();



        for( int i=0; i<nc; i++ ) {


        }
        t = core->getStartDate();

    }

    // Record the state as the state at the model start time.  This
    // will be repeatedly overwritten until the spinup is complete.
    cmodel->record_state(core->getStartDate());

    return spunup;
}

//------------------------------------------------------------------------------
/*! \brief visitor accept code
 */
void CarbonCycleSolver::accept( AVisitor* visitor )
{
    visitor->visit( this );
}

}
