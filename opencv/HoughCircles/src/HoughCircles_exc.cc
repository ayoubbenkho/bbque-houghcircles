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

using namespace cv;

Mat gray, img;
std::vector<Vec3f> circles;

HoughCircles::HoughCircles(std::string const & name,
		std::string filename,
		std::string const & recipe,
		RTLIB_Services_t *rtlib) :
	BbqueEXC(name, recipe, rtlib),filename(filename) {

	logger->Warn("New HoughCircles::HoughCircles()");
	logger->Info("EXC Unique IDentifier (UID): %u", GetUniqueID());

}

RTLIB_ExitCode_t HoughCircles::onSetup() {
	// This is just an empty method in the current implementation of this
	// testing application. However, this is intended to collect all the
	// application specific initialization code, especially the code which
	// acquire system resources (e.g. thread creation)
	logger->Warn("HoughCircles::onSetup()");

	img = imread(filename, IMREAD_COLOR);
	std::cout<<filename<<std::endl;
	if(img.empty())
    {
        logger->Fatal("Image not found.");
        return RTLIB_ERROR;
    }
	
	return RTLIB_OK;
}

RTLIB_ExitCode_t HoughCircles::onConfigure(int8_t awm_id) {

	logger->Warn("HoughCircles::onConfigure(): EXC [%s] => AWM [%02d]",
		exc_name.c_str(), awm_id);

	int32_t proc_quota, proc_nr, mem;
	GetAssignedResources(PROC_ELEMENT, proc_quota);
	GetAssignedResources(PROC_NR, proc_nr);
	GetAssignedResources(MEMORY, mem);
	logger->Notice("MayApp::onConfigure(): "
		"EXC [%s], AWM[%02d] => R<PROC_quota>=%3d, R<PROC_nr>=%2d, R<MEM>=%3d",
		exc_name.c_str(), awm_id, proc_quota, proc_nr, mem);

	return RTLIB_OK;
}

RTLIB_ExitCode_t HoughCircles::onRun() {
	RTLIB_WorkingModeParams_t const wmp = WorkingModeParams();
    
    cvtColor(img, gray, COLOR_BGR2GRAY);
    medianBlur(gray, gray, 5);
    cv::HoughCircles(gray, circles, HOUGH_GRADIENT, 1,
                 gray.rows/16, // change this value to detect circles with different distances to each other
                 100, 30, 1, 30 // change the last two parameters
                                // (min_radius & max_radius) to detect larger circles
                 );
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Vec3i c = circles[i];
        circle( img, Point(c[0], c[1]), c[2], Scalar(0,0,255), 3, LINE_AA);
        circle( img, Point(c[0], c[1]), 2, Scalar(0,255,0), 3, LINE_AA);
    }

	// Return after 5 cycles
	if (Cycles() >= 5)
		return RTLIB_EXC_WORKLOAD_NONE;


	// Do one more cycle
	logger->Warn("HoughCircles::onRun()      : EXC [%s]  @ AWM [%02d]",
		exc_name.c_str(), wmp.awm_id);

	return RTLIB_OK;
}

RTLIB_ExitCode_t HoughCircles::onMonitor() {
	RTLIB_WorkingModeParams_t const wmp = WorkingModeParams();

	logger->Warn("HoughCircles::onMonitor()  : EXC [%s]  @ AWM [%02d] "
			"=> cycles [%d], CPS = %.2f",
		exc_name.c_str(), wmp.awm_id, Cycles(), GetCPS());

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
