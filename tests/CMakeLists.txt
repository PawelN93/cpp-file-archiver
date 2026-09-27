add_executable(pax_tests
    test_pax.cpp
)

target_link_libraries(pax_tests
    PRIVATE
        pax_core
)

add_test(
    NAME pax_tests
    COMMAND pax_tests
)
