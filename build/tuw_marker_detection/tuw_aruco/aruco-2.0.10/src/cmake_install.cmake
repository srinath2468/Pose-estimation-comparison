# Install script for directory: /home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/srinath/catkin_thesis/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libaruco.so.2.0.9"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libaruco.so.2.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libaruco.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/srinath/catkin_thesis/devel/lib/libaruco.so.2.0.9"
    "/home/srinath/catkin_thesis/devel/lib/libaruco.so.2.0"
    "/home/srinath/catkin_thesis/devel/lib/libaruco.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libaruco.so.2.0.9"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libaruco.so.2.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libaruco.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/opt/ros/kinetic/lib/x86_64-linux-gnu:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/aruco" TYPE FILE FILES
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/ippe.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/exports.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/dictionary.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/cvdrawingutils.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/markerlabeler.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/aruco.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/marker.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/markermap.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/posetracker.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/ar_omp.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/levmarq.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/cameraparameters.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/markerdetector.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/markerlabelers/dictionary_based.h"
    "/home/srinath/catkin_thesis/src/tuw_marker_detection/tuw_aruco/aruco-2.0.10/src/markerlabelers/svmmarkers.h"
    )
endif()

