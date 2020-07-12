###############################################################################
# The AEM HoughCircles Application
################################################################################

cmake_minimum_required(VERSION 3.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#----- Check for the required RTLib library
find_package(BbqRTLib REQUIRED)

#----- Check for the required OpenCV library
find_package(OpenCV REQUIRED)

#----- Add compilation dependencies
include_directories(${BBQUE_RTLIB_INCLUDE_DIR} 
	            ${OpenCV_INCLUDE_DIRS}
)

#----- Add "houghcircles" target application
set(HOUGHCIRCLES_SRC version HoughCircles_exc HoughCircles_main)
add_executable(houghcircles ${HOUGHCIRCLES_SRC})

#----- Linking dependencies
target_link_libraries(
	houghcircles
	${Boost_LIBRARIES}
	${BBQUE_RTLIB_LIBRARY}
	${OpenCV_LIBS} 
)

# Use link path ad RPATH
set_property(TARGET houghcircles PROPERTY
	INSTALL_RPATH_USE_LINK_PATH TRUE)

#----- Install the HoughCircles files
install (TARGETS houghcircles RUNTIME
	DESTINATION ${HOUGHCIRCLES_PATH_BINS})

#----- Generate and Install HoughCircles configuration file
configure_file (
	"${PROJECT_SOURCE_DIR}/HoughCircles.conf.in"
	"${PROJECT_BINARY_DIR}/HoughCircles.conf"
)
install (FILES "${PROJECT_BINARY_DIR}/HoughCircles.conf"
	DESTINATION ${HOUGHCIRCLES_PATH_CONFIG})
