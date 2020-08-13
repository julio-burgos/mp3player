cmake_minimum_required(VERSION 3.0.0)
project(mp3player VERSION 0.1.0)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Curses REQUIRED)

add_executable(mp3player src/main.c)
target_link_libraries(mp3player ${CURSES_LIBRARIES} Threads::Threads)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
