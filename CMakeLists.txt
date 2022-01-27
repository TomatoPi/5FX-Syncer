cmake_minimum_required(VERSION 3.20.0)
project(5FX-Stepper VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS )
set(SOURCES src/5FX-Stepper.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# target_link_libraries(${PROJECT_NAME} JackWrap)
# target_link_libraries(${PROJECT_NAME} NSMWrap)
target_link_libraries(${PROJECT_NAME} jack)

target_include_directories(${PROJECT_NAME} PRIVATE src/)
install(TARGETS ${PROJECT_NAME})