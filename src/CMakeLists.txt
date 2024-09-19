include(Doxygen)

add_library(queue_lib INTERFACE queue.h)
target_include_directories(queue_lib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

add_executable(challenge main.cpp)
target_link_libraries(challenge PRIVATE queue_lib)

Doxygen(src docs)
