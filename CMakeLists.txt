cmake_minimum_required(VERSION 3.20.0)
project(5FX-Syncer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS 
    src/core/types.hpp
    src/core/engine.hpp
    src/core/hio.hpp
)
set(SOURCES 
    src/5FX-Syncer.cpp
)

add_subdirectory(src)
add_subdirectory(exp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# target_link_libraries(${PROJECT_NAME} JackWrap)
# target_link_libraries(${PROJECT_NAME} NSMWrap)
# target_link_libraries(${PROJECT_NAME} jack)
