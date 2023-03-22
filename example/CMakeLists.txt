include_directories(${PROJECT_SOURCE_DIR})

add_executable(generate generate.cpp)
target_link_libraries(generate TinyJSON)

add_executable(add_one add_one.cpp sample.h)
target_link_libraries(add_one TinyJSON)

add_executable(dom_parse_modify_stringify dom_parse_modify_stringify.cpp)
target_link_libraries(dom_parse_modify_stringify TinyJSON)
