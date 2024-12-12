cmake_minimum_required(VERSION 3.5.0)
project(pipe VERSION 0.1.0 LANGUAGES C)

set(SOURCE_FILES_1
    child.c
)

set(SOURCE_FILES_2
    parent.c
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GIO REQUIRED gio-2.0)

add_executable(child ${SOURCE_FILES_1})
add_executable(parent ${SOURCE_FILES_2})

target_include_directories(child PRIVATE ${GIO_INCLUDE_DIRS})
target_include_directories(parent PRIVATE ${GIO_INCLUDE_DIRS})
target_link_libraries(child PRIVATE ${GIO_LIBRARIES})
target_link_libraries(parent PRIVATE ${GIO_LIBRARIES})
target_compile_options(child PRIVATE ${GIO_CFLAGS})
target_compile_options(parent PRIVATE ${GIO_CFLAGS})