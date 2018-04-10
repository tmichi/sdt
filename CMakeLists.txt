CMAKE_MINIMUM_REQUIRED (VERSION 2.9)
SET(CMAKE_VERBOSE_MAKEFILE 1)
PROJECT(sdt)
SET ( CMAKE_MACOSX_RPATH 1 )
set ( CMAKE_BUILD_TYPE Release )
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
if (WIN32)
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else () # Cygwin, Mac
	add_definitions(-g -O3 -Wall)
	# -std=c++11 -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic)
	#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wendif-labels -Wfloat-equal -Winit-self -Winline -Wmissing-include-dirs -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-promo -Wswitch-default -Wswitch-enum -Wvariadic-macros -Wwrite-strings ")
	link_directories(/usr/local/lib)
	include_directories(./include)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
endif (WIN32)
ADD_SUBDIRECTORY(src)



