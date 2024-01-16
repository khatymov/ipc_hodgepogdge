cmake_minimum_required(VERSION 3.2)
project(my_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS "-lrt")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_executable(${PROJECT_NAME}
                main.cpp
                ${MY_CLASS}
                ${MOVE}
)

target_link_libraries(${PROJECT_NAME} -lrt -pthread)

add_subdirectory(tests)
