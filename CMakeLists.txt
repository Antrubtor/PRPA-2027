cmake_minimum_required (VERSION 3.18)
project (PRPA)



find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(benchmark REQUIRED)
find_package(fmt REQUIRED)


pkg_check_modules(GStreamer REQUIRED gstreamer-1.0 IMPORTED_TARGET)
pkg_check_modules(GStreamerVideo REQUIRED gstreamer-video-1.0 IMPORTED_TARGET)


#find_package(SDL2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(RESOURCES_DIR ${CMAKE_SOURCE_DIR})
configure_file(src/resources.cpp.in src/resources.cpp)


#add_library(JobQueue OBJECT src/JobQueue.hpp src/JobQueue.cpp)

add_library(stb_image src/stb_image.h src/stb_image.c)
add_library(Otsu src/Otsu.cpp src/logo.c)
target_link_libraries(Otsu PRIVATE Threads::Threads)

add_executable(stream src/stream.cpp src/gstfilter.c src/gstfilter.h)
target_link_libraries(stream PRIVATE PkgConfig::GStreamerVideo Otsu)

add_executable(bench src/bench.cpp  src/resources.cpp)
set_source_files_properties(src/resources.cpp PROPERTIES GENERATED TRUE)


target_link_libraries(bench PRIVATE benchmark::benchmark Otsu stb_image fmt::fmt)
