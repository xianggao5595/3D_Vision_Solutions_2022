cmake_minimum_required(VERSION 3.10)

project(Assignment_2_Solution)

find_package( OpenCV REQUIRED )

include_directories("include" 
		    "${OpenCV_INCLUDE_DIRS}"
	           )

file(GLOB SRCS
    "src/main.cpp"
    )

add_executable(Assignment_2_Solution ${SRCS})

target_link_libraries(Assignment_2_Solution ${OpenCV_LIBS})

install(TARGETS Assignment_2_Solution DESTINATION ${CMAKE_SOURCE_DIR}/bin)