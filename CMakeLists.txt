cmake_minimum_required(VERSION 3.10)

PROJECT(Anonymiser)

find_package(ITK REQUIRED)

set(SOURCE_FILES main.cc)

include_directories(${ITK_INCLUDE_DIRS})
include(${ITK_USE_FILE})

add_executable(anonymiser ${SOURCE_FILES})

target_link_libraries(anonymiser ${ITK_LIBRARIES})
