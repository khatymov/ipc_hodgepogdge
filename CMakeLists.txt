cmake_minimum_required(VERSION 3.2)
project(reader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

include(${CMAKE_CURRENT_LIST_DIR}/app/timer/timer.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/app/file_handler/file_handler.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/app/buffer/buffer.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/app/synchronizer/synchronizer.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/app/shared_memory_facade/shared_memory_facade.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/app/definitions/definitions.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/app/copier/copier.cmake)

add_executable(${PROJECT_NAME}
        main.cpp
        ${DEFINITIONS}
        ${FILE_HANDLER}
        ${TIMER}
        ${SYNCHRONIZER}
        ${BUFFER}
        ${COPIER}
        ${SHARED_MEMORY_FACADE}
)

target_link_libraries(${PROJECT_NAME} -lrt -pthread)

