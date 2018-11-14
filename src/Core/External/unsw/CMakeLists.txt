cmake_minimum_required(VERSION 2.8)
project(unsw)

#if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release)
#endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(qibuild)
include_directories(.)
include_directories(unsw)

file(GLOB_RECURSE UNSW_SRC
    "unsw/*.hpp"
    "unsw/*.cpp")

qi_create_lib(unsw STATIC ${UNSW_SRC})
qi_use_lib(unsw ZLIB BZIP2 boost_system boost_python boost_regex boost_thread boost_program_options boost_serialization ALCOMMON ALPROXIES ALVISION)
qi_stage_lib(unsw)




SET(AGENT_SRCS
   libagent/libagent.cpp
   libagent/libagent.hpp
   libagent/AgentData.hpp
   unsw/utils/options.cpp
)

qi_create_lib(agent MODULE ${AGENT_SRCS})
qi_stage_lib(agent)


#SET(SRC main.cpp)
#qi_create_bin(drop ${SRC})
#qi_use_lib(drop unsw)
