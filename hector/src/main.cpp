/* Hector -- A Simple Climate Model
   Copyright (C) 2014-2015  Battelle Memorial Institute

   Please see the accompanying file LICENSE.md for additional licensing
   information.
*/
/*
 *  main.cpp - wrapper entry point
 *  hector
 *
 *  Created by Ben on 9/14/10.
 *
 */

#include <iostream>

#include "core.hpp"
#include "logger.hpp"
#include "h_exception.hpp"
#include "h_util.hpp"
#include "h_reader.hpp"
#include "ini_to_core_reader.hpp"
#include "csv_outputstream_visitor.hpp"

#include "unitval.hpp"

using namespace std;

//-----------------------------------------------------------------------
/*! \brief Entry point for HECTOR wrapper.
 *
 *  Starting point for wrapper, not the core.
 */
int main (int argc, char * const argv[]) {
    using namespace Hector;

	try {
        // Create the Hector core
        Core core;
        Logger& glog = core.getGlobalLogger();


        // Parse the main configuration file
        if( argc > 1 ) {
            if( ifstream( argv[1] ) ) {
                h_reader reader( argv[1], INI_style );
            } else {

                H_THROW( "Couldn't find input file" )
            }
        } else {

            H_THROW( "Usage: <program> <config file name>" )
        }

        // Initialize the core and send input data to it

        core.init();


        INIToCoreReader coreParser( &core );
        coreParser.parse( argv[1] );

        // Create visitors

        filebuf csvoutputStreamFile;

        // Open the stream output file, which has an optional run name (specified in the INI file) in it
        string rn = core.getRun_name();
        if( rn == "" )
            csvoutputStreamFile.open( string( string( OUTPUT_DIRECTORY ) + "outputstream.csv" ).c_str(), ios::out );
        else
            csvoutputStreamFile.open( string( string( OUTPUT_DIRECTORY ) + "outputstream_" + rn + ".csv" ).c_str(), ios::out );


        ostream outputStream( &csvoutputStreamFile );
        CSVOutputStreamVisitor csvOutputStreamVisitor( outputStream );
        core.addVisitor( &csvOutputStreamVisitor );


        core.prepareToRun();


        core.run();


        glog.close();
    }
    catch( h_exception e ) {

        return 1;
    }
    catch( std::exception &e ) {

        return 2;
    }
    catch( ... ) {

        return 3;
    }

    return 0;
}
