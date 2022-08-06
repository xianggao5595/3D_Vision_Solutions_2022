# set the requirement for cmake version
cmake_minimum_required(VERSION 3.10)

# set the project name
project(Assignment_3_Solution)

# set the cmake source directory
set(3rdparty "${CMAKE_SOURCE_DIR}/3rdparty")

# External library
find_package( OpenCV REQUIRED )

# include all the head file
include_directories("include" 
	            "${3rdparty}/GraphCut"
		    "${OpenCV_INCLUDE_DIRS}"
	           )

# include all source file
file(GLOB SRCS
    "src/main.cpp"
    "${3rdparty}/GraphCut/maxflow.cpp"
    "${3rdparty}/GraphCut/graph.cpp"
    )

# execute all the source files
add_executable(Assignment_3_Solution ${SRCS})

# link to the opencv library
target_link_libraries(Assignment_3_Solution ${OpenCV_LIBS})

# install bin file under which the .exe appears
install(TARGETS Assignment_3_Solution DESTINATION ${CMAKE_SOURCE_DIR}/bin)