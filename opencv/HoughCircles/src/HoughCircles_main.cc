/**
 *       @file  HoughCircles_main.cc
 *      @brief  The HoughCircles BarbequeRTRM application
 *
 * Description: This program demonstrates circle finding with the Hough transform.
 *
 *     @author  Benkhoris Ayoub, ayoub.benkhoris@gmail.com
 *
 *     Company  Politecnico di Milano
 *   Copyright  Copyright (c) 2020, Ayoub Benkhoris
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * =====================================================================================
 */

#include <cstdio>
#include <iostream>
#include <random>
#include <cstring>
#include <memory>
#include <vector>

#include <libgen.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "version.h"
#include "HoughCircles_exc.h"
#include <bbque/utils/utility.h>
#include <bbque/utils/logging/logger.h>

// Setup logging
#undef  BBQUE_LOG_MODULE
#define BBQUE_LOG_MODULE "HoughCircles"

namespace bu = bbque::utils;
namespace po = boost::program_options;

/**
 * @brief A pointer to an EXC
 */
std::unique_ptr<bu::Logger> logger;

/**
 * @brief A pointer to an EXC
 */
typedef std::shared_ptr<BbqueEXC> pBbqueEXC_t;

/**
 * The decription of each HoughCircles parameters
 */
po::options_description opts_desc("HoughCircles Configuration Options");

/**
 * The map of all HoughCircles parameters values
 */
po::variables_map opts_vm;

/**
 * The services exported by the RTLib
 */
RTLIB_Services_t *rtlib;

/**
 * @brief The application configuration file
 */
std::string conf_file = BBQUE_PATH_PREFIX "/" BBQUE_PATH_CONF "/HoughCircles.conf" ;

/**
 * @brief The recipe to use for all the EXCs
 */
std::string recipe;

/**
 * @brief The image file to process
 */
std::string filename;

/**
 * @brief The EXecution Context (EXC) registered
 */
pBbqueEXC_t pexc;

int threads_number;
int upper_threshold;
int center_threshold;
int min_dist;
int max_radius;
int min_radius;
int jobs_number;

void ParseCommandLine(int argc, char *argv[]) {
	// Parse command line params
	try {
	po::store(po::parse_command_line(argc, argv, opts_desc), opts_vm);
	} catch(...) {
		std::cout << "Usage: " << argv[0] << " [options]\n";
		std::cout << opts_desc << std::endl;
		::exit(EXIT_FAILURE);
	}
	po::notify(opts_vm);

	// Check for help request
	if (opts_vm.count("help")) {
		std::cout << "Usage: " << argv[0] << " [options]\n";
		std::cout << opts_desc << std::endl;
		::exit(EXIT_SUCCESS);
	}

	// Check for version request
	if (opts_vm.count("version")) {
		std::cout << "HoughCircles (ver. " << g_git_version << ")\n";
		std::cout << "Copyright (C) 2020 Politecnico di Milano\n";
		std::cout << "\n";
		std::cout << "Built on " <<
			__DATE__ << " " <<
			__TIME__ << "\n";
		std::cout << "\n";
		std::cout << "This is free software; see the source for "
			"copying conditions.  There is NO\n";
		std::cout << "warranty; not even for MERCHANTABILITY or "
			"FITNESS FOR A PARTICULAR PURPOSE.";
		std::cout << "\n" << std::endl;
		::exit(EXIT_SUCCESS);
	}
}

int main(int argc, char *argv[]) {

	opts_desc.add_options()
		("help,h", "print this help message")
		("version,v", "print program version")

		("conf,C", po::value<std::string>(&conf_file)->
			default_value(conf_file),
			"HoughCircles configuration file")

		("recipe,r", po::value<std::string>(&recipe)->
			default_value("HoughCircles"),
			"recipe name (for all EXCs)")

		("filename,f", po::value<std::string>(&filename)-> 
			default_value("jo.jpg"), 
			"Image to process")

		("threads,t", po::value<int>(&threads_number)->
			default_value(1),
			"Number of threads to exploit")

		("center_threshold,c", po::value<int>(&center_threshold)->
			default_value(30),
			"Threshold for center detection")

		("upper_threshold,u", po::value<int>(&upper_threshold)->
			default_value(100),
			"Upper threshold for the internal Canny edge detector")

		("jobs,j", po::value<int>(&jobs_number)->
			default_value(50),
			"Number of jobs to perform")

		("min_radius,i", po::value<int>(&min_radius)->
			default_value(1),
			"Minimum radius to be detected")

		("max_radius,a", po::value<int>(&max_radius)->
			default_value(30),
			"Maximum radius to be detected")

		("min_dist,m", po::value<int>(&min_dist)->
			default_value(16),
			"Minimum distance between detected centers")

	;

	// Setup a logger
	bu::Logger::SetConfigurationFile(conf_file);
	logger = bu::Logger::GetLogger("houghcircles");

	ParseCommandLine(argc, argv);

	// Welcome screen
	logger->Info(".:: HoughCircles (ver. %s) ::.", g_git_version);
	logger->Info("Built: " __DATE__  " " __TIME__);

	// Initializing the RTLib library and setup the communication channel
	// with the Barbeque RTRM
	logger->Info("STEP 0. Initializing RTLib, application [%s]...",
			::basename(argv[0]));

	if ( RTLIB_Init(::basename(argv[0]), &rtlib) != RTLIB_OK) {
		logger->Fatal("Unable to init RTLib (Did you start the BarbequeRTRM daemon?)");
		return RTLIB_ERROR;
	}

	assert(rtlib);
	logger->Info("STEP 1. Registering EXC with recipe <%s>...", recipe.c_str());

	if (upper_threshold > 500) upper_threshold = 500;
	if (center_threshold > 250) center_threshold = 250;
	if (min_radius < 0) min_radius = 0;
	if (max_radius > 100) max_radius = 100;
	if (min_dist > 64) max_radius = 64;

	pexc = std::make_shared<HoughCircles>("HoughCircles", filename, threads_number, upper_threshold, center_threshold, min_dist, max_radius, min_radius, jobs_number, recipe, rtlib);
	if (!pexc->isRegistered()) {
		logger->Fatal("Registering failure.");
		return RTLIB_ERROR;
	}


	logger->Info("STEP 2. Starting EXC control thread...");
	pexc->Start();


	logger->Info("STEP 3. Waiting for EXC completion...");
	pexc->WaitCompletion();


	logger->Info("STEP 4. Disabling EXC...");
	pexc = NULL;

	logger->Info("===== HoughCircles DONE! =====");
	return EXIT_SUCCESS;

}

