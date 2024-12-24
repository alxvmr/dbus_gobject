cmake_minimum_required(VERSION 3.5.0)
project(passwdservice VERSION 0.1.0 LANGUAGES C)

set(SOURCE_FILES_PASSWD_SERVICE
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/passwdservice.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/passwduser.c
)

set(SOURCE_FILES_PAM_HELPER
    ${CMAKE_CURRENT_SOURCE_DIR}/source/pam_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/translate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/passwduser.c
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)
pkg_check_modules(DBUS_GLIB REQUIRED dbus-glib-1)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(PAM REQUIRED pam)
pkg_check_modules(PAM_MISC pam_misc)
pkg_check_modules(JSON_GLIB json-glib-1.0)

add_executable(passwd_service ${SOURCE_FILES_PASSWD_SERVICE})
add_executable(pam_helper  ${SOURCE_FILES_PAM_HELPER})

target_include_directories(passwd_service PRIVATE ${HEADER_DIR} ${GOBJECT_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS} ${DBUS_GLIB_INCLUDE_DIRS} ${DBUS_INCLUDE_DIRS})
target_link_libraries(passwd_service PRIVATE ${GOBJECT_LIBRARIES} ${GIO_LIBRARIES} ${DBUS_GLIB_LIBRARIES} ${DBUS_LIBRARIES})
target_compile_options(passwd_service PRIVATE ${GOBJECT_CFLAGS} ${GIO_CFLAGS} ${DBUS_GLIB_CFLAGS} ${DBUS_CFLAGS})

target_include_directories(pam_helper PRIVATE ${HEADER_DIR} ${GOBJECT_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS} ${PAM_INCLUDE_DIRS} ${JSON_GLIB_DIRS})
target_link_libraries(pam_helper PRIVATE ${GOBJECT_LIBRARIES} ${GIO_LIBRARIES} ${PAM_LIBRARIES} ${JSON_GLIB_LIBRARIES})
target_compile_options(pam_helper PRIVATE ${GOBJECT_CFLAGS} ${GIO_CFLAGS} ${PAM_CFLAGS} ${JSON_GLIB_CFLAGS})

set(CMAKE_CLEAN_FILES ${PROJECT_NAME})
include(GNUInstallDirs)
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/${PROJECT_NAME}")
