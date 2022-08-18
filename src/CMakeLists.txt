function(add_module MODULE_NAME ADDITIONAL_SOURCES)
    set(EXEC_NAME TestSyncer${MODULE_NAME})
    add_executable(${EXEC_NAME} ${ADDITIONAL_SOURCES} ${MODULE_NAME}.hpp tests/${MODULE_NAME}.cpp)
    add_test(NAME ${EXEC_NAME} COMMAND ${EXEC_NAME})
endfunction()

add_subdirectory(core)
