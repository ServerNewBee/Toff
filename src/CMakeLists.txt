include_directories("../libs")

add_executable(simple_server simple_server.cpp)
target_link_libraries(simple_server parser)
target_link_libraries(simple_server types)
