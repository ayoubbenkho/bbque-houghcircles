/**
 *       @file  HoughCircles_exc.h
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

#ifndef HOUGHCIRCLES_EXC_H_
#define HOUGHCIRCLES_EXC_H_

#include <bbque/bbque_exc.h>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using bbque::rtlib::BbqueEXC;

class HoughCircles : public BbqueEXC {


std::string filename;
int threads_number;
int upper_threshold;
int center_threshold;
int min_radius;
int max_radius;
int min_dist;
int jobs_number;
unsigned int cps_goal;
int jobs_done = 0;
std::vector<std::thread> threads_container;

public:

	HoughCircles(std::string const & name,
			std::string filename,
			int threads_number,
			int upper_threshold,
			int center_threshold,
			int min_dist,
			int max_radius,
			int min_radius,
			int jobs_number,
			unsigned int cps_goal,
			std::string const & recipe,
			RTLIB_Services_t *rtlib);

private:

	RTLIB_ExitCode_t onSetup();
	RTLIB_ExitCode_t onConfigure(int8_t awm_id);
	RTLIB_ExitCode_t onRun();
	RTLIB_ExitCode_t onMonitor();
	RTLIB_ExitCode_t onSuspend();
	RTLIB_ExitCode_t onRelease();
	size_t Work();
};

#endif // HOUGHCIRCLES_EXC_H_
