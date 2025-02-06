include(AddCatch2)

FILE(GLOB_RECURSE SRC "*.cpp")
add_executable(tests ${SRC})
target_link_libraries(tests PRIVATE ossrc Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tests)

add_custom_target(run_tests
        COMMAND tests
        DEPENDS tests
)