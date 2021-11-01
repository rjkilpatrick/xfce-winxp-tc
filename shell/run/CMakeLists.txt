cmake_minimum_required(VERSION 2.8)

project(run)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GDK        REQUIRED gdk-3.0)
pkg_check_modules(GLIB       REQUIRED glib-2.0)
pkg_check_modules(GTK3       REQUIRED gtk+-3.0)
pkg_check_modules(WINTC_EXEC REQUIRED wintc-exec)

include_directories(
    ${GDK_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
    ${WINTC_EXEC_INCLUDE_DIRS})

link_directories(
    ${GDK_LIBRARY_DIRS}
    ${GLIB_LIBRARY_DIRS}
    ${GTK3_LIBRARY_DIRS}
    ${WINTC_EXEC_LIBRARY_DIRS})

add_executable(run
    src/run.c)

target_link_libraries(run
    PRIVATE ${GDK_LIBRARIES}
    PRIVATE ${GLIB_LIBRARIES}
    PRIVATE ${GTK3_LIBRARIES}
    PRIVATE ${WINTC_EXEC_LIBRARIES})
