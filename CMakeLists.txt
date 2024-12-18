cmake_minimum_required(VERSION 3.5.0)
project(alxvmrexample VERSION 0.1.0 LANGUAGES C)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PROGNAME ${PROJECT_NAME})

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)
pkg_check_modules(DBUS_GLIB REQUIRED dbus-glib-1)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)

add_executable(alxvmrexample ${SOURCE_DIR}/main.c ${SOURCE_DIR}/alxvmrexample.c)

target_compile_definitions(${PROGNAME} PRIVATE PROGNAME="${PROGNAME}")

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADER_DIR} ${GOBJECT_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS} ${DBUS_GLIB_INCLUDE_DIRS} ${DBUS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GOBJECT_LIBRARIES} ${GIO_LIBRARIES} ${DBUS_GLIB_LIBRARIES} ${DBUS_LIBRARIES})

target_compile_options(${PROJECT_NAME} PRIVATE ${GOBJECT_CFLAGS} ${GIO_CFLAGS} ${DBUS_GLIB_CFLAGS} ${DBUS_CFLAGS})

set(CMAKE_CLEAN_FILES ${PROJECT_NAME})

include(GNUInstallDirs)
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/${PROJECT_NAME}")
