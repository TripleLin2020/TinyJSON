include_directories(${PROJECT_SOURCE_DIR}/TinyJSON)

add_executable(parse parse.cpp)
target_link_libraries(parse TinyJSON)

add_executable(generate generate.cpp)
target_link_libraries(generate TinyJSON)

add_executable(print_document print_document.cpp)
target_link_libraries(print_document TinyJSON)

add_executable(add_one add_one.cpp sample.h)
target_link_libraries(add_one TinyJSON)

add_executable(write_document write_document.cpp)
target_link_libraries(write_document TinyJSON)

add_executable(struct struct.cpp)
target_link_libraries(struct TinyJSON)
