add_executable(parse parse.cc)
target_link_libraries(parse TinyJSON)

add_executable(generate generate.cc)
target_link_libraries(generate TinyJSON)

add_executable(print_document print_document.cc)
target_link_libraries(print_document TinyJSON)

add_executable(add_one add_one.cc sample.h)
target_link_libraries(add_one TinyJSON)

add_executable(pretty pretty.cc)
target_link_libraries(pretty TinyJSON)

add_executable(write_document write_document.cc)
target_link_libraries(write_document TinyJSON)

add_executable(struct struct.cc)
target_link_libraries(struct TinyJSON)
