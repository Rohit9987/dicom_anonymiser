cmake_minimum_required(VERSION 3.10)

PROJECT(Anonymiser)

find_package(ITK REQUIRED)

set(SOURCE_FILES main.cc)

include_directories(${ITK_INCLUDE_DIRS})
include(${ITK_USE_FILE})

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})

add_executable(anonymiser ${SOURCE_FILES})

target_link_libraries(anonymiser ${ITK_LIBRARIES} ${JSONCPP_LIBRARIES})
set_property(TARGET anonymiser PROPERTY CXX_STANDARD 17)
