cmake_minimum_required(VERSION 3.20)

# Single test executable that contains all small unit tests
add_executable(EventStreamTests
    ConfigLoaderTest.cpp
    EventTest.cpp
)

target_include_directories(EventStreamTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(EventStreamTests
    PRIVATE
        config
        events
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(EventStreamTests)

add_test(NAME EventStreamTests COMMAND EventStreamTests)
