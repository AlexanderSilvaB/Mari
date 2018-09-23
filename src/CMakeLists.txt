cmake_minimum_required(VERSION 2.8)
project(CodeRelease)

add_definitions( " -DVERSION_MAJOR=1 ")
add_definitions( " -DVERSION_MINOR=0 ")

option(USE_QIBUILD "module is compiled using qibuild (ON or OFF)" OFF)
option(USE_UNSW "module is compiled using unsw (ON or OFF)" ON)
option(USE_V4L2 "module is compiled using v4l2 camera driver (ON or OFF)" ON)

add_definitions(-Wall -fpermissive)

find_package(qibuild)

if(USE_QIBUILD)
    add_definitions( " -DUSE_QIBUILD ")
else()
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDES})
endif()

if(USE_V4L2)
    add_definitions( " -DUSE_V4L2 ")
endif()

if(USE_UNSW)
    add_definitions( " -DUSE_UNSW ")
    include_directories("Core/External/unsw")
    include_directories("Core/External/unsw/unsw")
endif()

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

file(GLOB_RECURSE MODULES_SRC
	"Modules/*.cpp"
    "Modules/*.c"
	"Modules/*.h"
    "Modules/*.hpp")

add_subdirectory("Core/External")

qi_create_bin(rinobot ${src} ${CORE_SRC} ${CORE_DRIVERS_SRC} ${CORE_UTILS_SRC} ${MODULES_SRC})
qi_use_lib(rinobot RinoLib unsw )
target_link_libraries(rinobot pthread ${OpenCV_LIBS} TcpUdpSocket)
