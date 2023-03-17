include_directories(${PROJECT_SOURCE_DIR}/third_party/benchmark/include)
add_executable(bench_parse bench_parse.cc)
target_link_libraries(bench_parse TinyJSON benchmark pthread)

