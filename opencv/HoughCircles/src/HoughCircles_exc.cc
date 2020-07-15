/**
 *       @file  HoughCircles_exc.cc
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


#include "HoughCircles_exc.h"

#include <cstdio>
#include <bbque/utils/utility.h>

#include <math.h>

using namespace cv;

Mat gray, img;
std::vector<Vec3f> circles;

HoughCircles::HoughCircles(std::string const & name,
		std::string filename,
		int threads_number,
		int upper_threshold,
		int center_threshold,
		int min_dist,
		int max_radius,
		int min_radius,
		int jobs_number,
		bool slow_mode,
		std::string const & recipe,
		RTLIB_Services_t *rtlib) :
	BbqueEXC(name, recipe, rtlib),
	filename(filename),
	jobs_number(jobs_number),
	center_threshold(center_threshold),
	upper_threshold(upper_threshold),
	min_radius(min_radius),
	min_dist(min_dist),
	max_radius(max_radius),
	threads_number(threads_number),
	slow_mode(slow_mode) {

	logger->Warn("New HoughCircles::HoughCircles()");
	logger->Info("EXC Unique IDentifier (UID): %u", GetUniqueID());

}

RTLIB_ExitCode_t HoughCircles::onSetup() {

	logger->Warn("HoughCircles::onSetup()");

	if (slow_mode)
		SetCPSGoal(1,2);

	img = imread(filename, IMREAD_COLOR);
	if(img.empty())
    {
        logger->Fatal("Image not found.");
        return RTLIB_ERROR;
    }
	cvtColor(img, gray, COLOR_BGR2GRAY);
    medianBlur(gray, gray, 5);
	
	return RTLIB_OK;
}

RTLIB_ExitCode_t HoughCircles::onConfigure(int8_t awm_id) {

	logger->Warn("HoughCircles::onConfigure(): EXC [%s] => AWM [%02d]", 
		exc_name.c_str(), awm_id);

	int32_t proc_quota, proc_nr, mem, gpu_nr;
	GetAssignedResources(PROC_ELEMENT, proc_quota);
	GetAssignedResources(PROC_NR, proc_nr);
	GetAssignedResources(MEMORY, mem);
	GetAssignedResources(GPU, gpu_nr);


	logger->Notice("HoughCircles::onConfigure(): "
		"EXC [%s], AWM[%02d] => R<PROC_quota>=%3d, R<PROC_nr>=%2d, R<MEM>=%3d, R<GPU>=%3d",
		exc_name.c_str(), awm_id, proc_quota, proc_nr, mem, gpu_nr);

	if (proc_quota >= 100)
		threads_number = proc_quota / 100;
	else if (proc_quota != -1)
		threads_number = 1;

	logger->Notice("HoughCircles: %d thread(s)", threads_number);

	return RTLIB_OK;
}

RTLIB_ExitCode_t HoughCircles::onRun() {
	RTLIB_WorkingModeParams_t const wmp = WorkingModeParams();
 
    for (int i = 0; i < threads_number; ++i)
    	threads_container.push_back(std::thread(&HoughCircles::Work, this));

    for (auto &thread : threads_container)
    	thread.join();

    threads_container.clear();

	// Do one more cycle
	logger->Warn("HoughCircles::onRun()      : EXC [%s]  @ AWM [%02d]",
		exc_name.c_str(), wmp.awm_id);

	return RTLIB_OK;
}

RTLIB_ExitCode_t HoughCircles::onMonitor() {
	RTLIB_WorkingModeParams_t const wmp = WorkingModeParams();

	int quality_of_service = max_radius - min_radius - min_dist;

	if (quality_of_service < 10)
		logger->Notice("[onMonitor]: Low QoS (%d) on cycle %d",
						quality_of_service, Cycles());
	else if ( quality_of_service < 20 )
		logger->Notice("[onMonitor]: Medium QoS (%d) on cycle %d",
						quality_of_service, Cycles());
	else
		logger->Notice("[onMonitor]: High QoS (%d) on cycle %d",
						quality_of_service, Cycles());      


	logger->Warn("HoughCircles::onMonitor()  : EXC [%s]  @ AWM [%02d] "
			"=> cycles [%d], CPS = %.2f",
		exc_name.c_str(), wmp.awm_id, Cycles(), GetCPS());

	if (jobs_done >= jobs_number) return RTLIB_EXC_WORKLOAD_NONE;

	if (jobs_number - jobs_done < threads_number)
		threads_number = jobs_number - jobs_done;

	return RTLIB_OK;
}

RTLIB_ExitCode_t HoughCircles::onSuspend() {

	logger->Warn("HoughCircles::onSuspend()  : suspension...");

	return RTLIB_OK;
}

RTLIB_ExitCode_t HoughCircles::onRelease() {

	imshow("detected circles", img);
    waitKey();

	logger->Warn("HoughCircles::onRelease()  : exit");

	return RTLIB_OK;
}

size_t HoughCircles::Work(){

	cv::HoughCircles(gray, circles, HOUGH_GRADIENT, 1,
                 gray.rows/min_dist, 
                 upper_threshold, center_threshold, 
                 min_radius, max_radius
                 );
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Vec3i c = circles[i];
        circle( img, Point(c[0], c[1]), c[2], Scalar(0,0,255), 3, LINE_AA);
        circle( img, Point(c[0], c[1]), 2, Scalar(0,255,0), 3, LINE_AA);
    }

    jobs_done++;

    return circles.size();
}
