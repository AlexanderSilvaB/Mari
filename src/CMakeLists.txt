cmake_minimum_required(VERSION 2.8)
project(CodeRelease)

#if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release)
#endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_definitions( " -DVERSION_MAJOR=1 ")
add_definitions( " -DVERSION_MINOR=0 ")

add_definitions(-Wall -fpermissive)

message(INFO " Mode: ${CMAKE_BUILD_TYPE}")

find_package(qibuild)

#find_package(OpenCV REQUIRED)
#find_package(OPENCV2_CORE)
#find_package(OPENCV2_IMGPROC)
#find_package(OPENCV2_OBJDETECT)


#set(OpenCV_INCLUDES_DIRS 
#    ${OpenCV_INCLUDES}  
#    ${OPENCV2_CORE_INCLUDE_DIRS}
#    ${OPENCV2_IMGPROC_INCLUDE_DIRS}
#    ${OPENCV2_OBJDETECT_INCLUDE_DIRS})

#set(OpenCV_LIBS_DIRS 
#    ${OpenCV_LIBS}
#    ${OPENCV2_CORE_LIBRARIES}
#    ${OPENCV2_IMGPROC_LIBRARIES}
#    ${OPENCV2_OBJDETECT_LIBRARIES})

#include_directories(${OpenCV_INCLUDES_DIRS})

include_directories("Core/External/unsw")
include_directories("Core/External/unsw/unsw")

include_directories("Core/External/naoleague")

include_directories(".")
include_directories("Modules")
set(src
    main.cpp
)

set(CORE_SRC
    "Core/InitManager.cpp"
    "Core/Module.cpp"
    "Core/ModulesManager.cpp"
    "Core/SpellBook.cpp")

file(GLOB_RECURSE CORE_DRIVERS_SRC
	"Core/Drivers/*.cpp"
    "Core/Drivers/*.c"
	"Core/Drivers/*.h"
    "Core/Drivers/*.hpp")

file(GLOB_RECURSE CORE_UTILS_SRC
	"Core/Utils/*.cpp"
    "Core/Utils/*.c"
	"Core/Utils/*.h"
    "Core/Utils/*.hpp")

file(GLOB_RECURSE CORE_GC_SRC
	"Core/GC/*.cpp"
    "Core/GC/*.c"
	"Core/GC/*.h"
    "Core/GC/*.hpp")

file(GLOB_RECURSE MODULES_SRC
	"Modules/*.cpp"
    "Modules/*.c"
	"Modules/*.h"
    "Modules/*.hpp")

add_subdirectory("Core/External")

qi_create_bin(rinobot ${src} ${CORE_SRC} ${CORE_DRIVERS_SRC} ${CORE_UTILS_SRC} ${CORE_GC_SRC} ${MODULES_SRC})
qi_use_lib(rinobot unsw naoleague    
        OPENCV2_CORE 
        OPENCV2_HIGHGUI 
        OPENCV2_IMGPROC 
        OPENCV2_VIDEO 
        OPENCV2_OBJDETECT )
target_link_libraries(rinobot pthread ${OpenCV_LIBS_DIRS} TcpUdpSocket naoleague)
