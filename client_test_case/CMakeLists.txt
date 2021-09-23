cmake_minimum_required(VERSION 3.5)
project(project)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/client CLIENT_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/server SERVER_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/open62541 OPEN62541_SRC)
add_executable(server ${SERVER_SRC} ${OPEN62541_SRC} ${CLIENT_SRC})
# add_executable(client ${CLIENT_SRC} ${OPEN62541_SRC})

add_executable(main ${PROJECT_SOURCE_DIR}/src/main.c ${CLIENT_SRC} ${OPEN62541_SRC})
target_link_libraries(main check)