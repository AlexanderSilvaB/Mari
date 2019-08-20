cmake_minimum_required(VERSION 2.8)
project(naoleague)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

file(GLOB_RECURSE NAOLEAGUE_SRC
    "featureextraction/*.h"
    "featureextraction/*.cpp")

qi_create_lib(naoleague STATIC ${NAOLEAGUE_SRC})
qi_use_lib(naoleague   
        OPENCV2_CORE 
        OPENCV2_HIGHGUI 
        OPENCV2_IMGPROC 
        OPENCV2_VIDEO 
        OPENCV2_OBJDETECT )

qi_stage_lib(naoleague)
target_link_libraries(naoleague ${OpenCV_LIBS})