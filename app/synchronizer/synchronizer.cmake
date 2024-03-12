include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/semaphore_proxy/semaphore_proxy.cmake)

set(SYNCHRONIZER
        ${SEMAPHORE_PROXY}
        ${CMAKE_CURRENT_LIST_DIR}/synchronizer.h
        ${CMAKE_CURRENT_LIST_DIR}/synchronizer.cpp
)
