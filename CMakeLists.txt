cmake_minimum_required(VERSION 3.2)
project(my_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

include(${CMAKE_CURRENT_LIST_DIR}/shared_memory_handler/shared_memory_handler.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/buffer_rotator/buffer_rotator.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/buffer/buffer.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/file_handler/file_handler.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/timer/timer.cmake)

add_executable(${PROJECT_NAME}
                main.cpp
                ${BUFFER}
                ${SHARED_MEMORY_HANDLER}
                ${BUFFER_ROTATOR}
                ${FILE_HANDLER}
                ${TIMER}
)

target_link_libraries(${PROJECT_NAME} -lrt -pthread)

