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

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# target_link_libraries(${PROJECT_NAME} JackWrap)
# target_link_libraries(${PROJECT_NAME} NSMWrap)
# target_link_libraries(${PROJECT_NAME} jack)

add_executable(TestSyncerTime src/core/time.hpp tests/time.cpp)
add_test(NAME TestSyncerTime COMMAND TestSyncerTime)

add_executable(TestSyncerInterval src/core/interval.hpp tests/interval.cpp)
add_test(NAME TestSyncerInterval COMMAND TestSyncerInterval)

add_executable(TestSyncerCore src/core/types.hpp tests/core.cpp)
add_test(NAME TestSyncerCore COMMAND TestSyncerCore)

add_executable(TestSyncerEngine src/core/types.hpp src/core/engine.hpp tests/engine.cpp)
add_test(NAME TestSyncerEngine COMMAND TestSyncerEngine)

add_executable(TestSyncerLoad src/core/types.hpp src/core/hio.hpp tests/load.cpp)
add_test(NAME TestSyncerLoad COMMAND TestSyncerLoad)
